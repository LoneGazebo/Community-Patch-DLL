/*	-------------------------------------------------------------------------------------------------------
Opens an IPC channel that exposes the game's internal state to external services.
	------------------------------------------------------------------------------------------------------- */

#include "CvGameCoreDLLPCH.h"
#include "CvConnectionService.h"
#include "CvGlobals.h"
#include "CvGame.h"
#include "CustomMods.h"
#include "FireWorks/FILogFile.h"
#include "ICvDLLScriptSystem.h"
#include "Lua/CvLuaSupport.h"
#include <sstream>
#include <iomanip>
#include <cmath>
#include <climits>

// Note: Buffer allocation moved to Setup() to avoid unnecessary memory usage when IPC is disabled

// Singleton instance getter
CvConnectionService& CvConnectionService::GetInstance()
{
	static CvConnectionService instance;
	return instance;
}

// Constructor
CvConnectionService::CvConnectionService()
	: m_bInitialized(false)
	, m_pLuaState(NULL)
	, m_hPipe(INVALID_HANDLE_VALUE)
	, m_hThread(NULL)
	, m_dwThreadId(0)
	, m_bClientConnected(false)
	, m_bShutdownRequested(false)
	, m_uiNextCallId(1)
	, m_uiCurrentTurn(0)
	, m_uiEventSequence(1)
	, m_pMainThreadReadBuffer(NULL)
	, m_pMainThreadWriteBuffer(NULL)
{
}

// Destructor
CvConnectionService::~CvConnectionService()
{
	if (m_bInitialized)
	{
		Shutdown();
	}
}

// Initialize the connection service
bool CvConnectionService::Setup()
{
	if (m_bInitialized)
	{
		Log(LOG_WARNING, "ConnectionService::Setup() called when already initialized");
		return true;
	}

	Log(LOG_INFO, "ConnectionService::Setup() - Initializing connection service");

	// Allocate the JSON buffers (40KB each)
	m_pMainThreadReadBuffer = new DynamicJsonDocument(40960);
	m_pMainThreadWriteBuffer = new DynamicJsonDocument(40960);

	// Initialize critical sections for thread safety
	InitializeCriticalSection(&m_csIncoming);
	InitializeCriticalSection(&m_csOutgoing);
	InitializeCriticalSection(&m_csFunctions);
	InitializeCriticalSection(&m_csExternalFunctions);
	InitializeCriticalSection(&m_csPendingCalls);

	// Reset shutdown flag
	m_bShutdownRequested = false;
	m_bClientConnected = false;

	// Initialize Lua state for script execution
	m_pLuaState = gDLL->GetScriptSystem()->CreateLuaThread("ConnectionService");

	// Reading counters
	m_uiCurrentTurn = GC.getGame().getGameTurn();
	DeserializeEventSequence();

	// Register game data with the Lua state
	LuaSupport::RegisterScriptData(m_pLuaState);
	Log(LOG_INFO, "ConnectionService::Setup() - Lua state initialized, creating named pipe server");
	
	// Create the Named Pipe server thread
	m_hThread = CreateThread(
		NULL,                   // default security attributes
		0,                      // default stack size
		NamedPipeServerThread,  // thread function
		this,                   // parameter to thread function
		0,                      // default creation flags
		&m_dwThreadId);         // thread identifier
	
	if (m_hThread == NULL)
	{
		std::stringstream ss;
		ss << "ConnectionService::Setup() - Failed to create thread, error: " << GetLastError();
		Log(LOG_ERROR, ss.str().c_str());
		return false;
	}

	m_bInitialized = true;
	std::stringstream ss;
	ss << "ConnectionService::Setup() - Successfully initialized with thread ID: " << m_dwThreadId;
	Log(LOG_INFO, ss.str().c_str());
	
	return true;
}

// Shutdown the connection service and cleanup resources
void CvConnectionService::Shutdown()
{
	if (!m_bInitialized)
	{
		return;
	}

	Log(LOG_INFO, "ConnectionService::Shutdown() - Shutting down connection service");
	
	// Save event sequence before shutting down
	SerializeEventSequence();

	// Signal the thread to shutdown
	m_bShutdownRequested = true;
	
	// Force disconnect any connected client first
	m_bClientConnected = false;

	// Close the pipe if it's open to wake up the thread
	if (m_hPipe != INVALID_HANDLE_VALUE)
	{
		// Cancel any pending I/O operations
		CancelIo(m_hPipe);
		
		// Disconnect and close the pipe
		DisconnectNamedPipe(m_hPipe);
		CloseHandle(m_hPipe);
		m_hPipe = INVALID_HANDLE_VALUE;
	}

	// Wait for the thread to finish (max 5 seconds)
	if (m_hThread != NULL)
	{
		Log(LOG_INFO, "ConnectionService::Shutdown() - Waiting for thread to terminate");
		DWORD waitResult = WaitForSingleObject(m_hThread, 5000);
		
		if (waitResult == WAIT_TIMEOUT)
		{
			Log(LOG_WARNING, "ConnectionService::Shutdown() - Thread did not terminate gracefully, forcing termination");
			TerminateThread(m_hThread, 1);
		}
		else if (waitResult == WAIT_OBJECT_0)
		{
			Log(LOG_INFO, "ConnectionService::Shutdown() - Thread terminated gracefully");
		}
		
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}

	// Clean up Lua state
	if (m_pLuaState)
	{
		gDLL->GetScriptSystem()->FreeLuaThread(m_pLuaState);
		m_pLuaState = NULL;
		Log(LOG_INFO, "ConnectionService::Shutdown() - Lua state freed");
	}

	// Clear all registered Lua functions
	EnterCriticalSection(&m_csFunctions);
	m_registeredFunctions.clear();
	LeaveCriticalSection(&m_csFunctions);
	
	// Clear any pending external calls
	EnterCriticalSection(&m_csPendingCalls);
	m_pendingExternalCalls.clear();
	m_syncCallResults.clear();
	LeaveCriticalSection(&m_csPendingCalls);

	// Clean up critical sections
	DeleteCriticalSection(&m_csIncoming);
	DeleteCriticalSection(&m_csOutgoing);
	DeleteCriticalSection(&m_csFunctions);
	DeleteCriticalSection(&m_csExternalFunctions);
	DeleteCriticalSection(&m_csPendingCalls);

	// Free the JSON buffers
	if (m_pMainThreadReadBuffer)
	{
		delete m_pMainThreadReadBuffer;
		m_pMainThreadReadBuffer = NULL;
	}
	if (m_pMainThreadWriteBuffer)
	{
		delete m_pMainThreadWriteBuffer;
		m_pMainThreadWriteBuffer = NULL;
	}

	// Clear any remaining messages in the queues
	while (!m_incomingQueue.empty())
	{
		m_incomingQueue.pop();
	}
	while (!m_outgoingQueue.empty())
	{
		m_outgoingQueue.pop();
	}

	m_bInitialized = false;
	m_bClientConnected = false;
	Log(LOG_INFO, "ConnectionService::Shutdown() - Successfully shut down");
}

// Log a message to the connection log file
void CvConnectionService::Log(LogLevel level, const char* message)
{
	// Only log if logging is enabled in the game
	if (!GC.getLogging())
	{
		return;
	}

	FILogFile* pLog = GetLogFile();
	if (pLog)
	{
		std::stringstream ss;
		
		// Add timestamp
		SYSTEMTIME st;
		GetLocalTime(&st);
		ss << "[" << std::setfill('0') << std::setw(2) << st.wHour << ":"
		   << std::setfill('0') << std::setw(2) << st.wMinute << ":"
		   << std::setfill('0') << std::setw(2) << st.wSecond << "."
		   << std::setfill('0') << std::setw(3) << st.wMilliseconds << "] ";
		
		// Add turn number prefix
		ss << "[Turn " << GC.getGame().getElapsedGameTurns() << "] ";
		
		// Add log level prefix
		switch (level)
		{
		case LOG_DEBUG:
			ss << "[DEBUG] ";
			break;
		case LOG_INFO:
			ss << "[INFO] ";
			break;
		case LOG_WARNING:
			ss << "[WARNING] ";
			break;
		case LOG_ERROR:
			ss << "[ERROR] ";
			break;
		default:
			ss << "[UNKNOWN] ";
			break;
		}
		
		// Add the actual message
		ss << message;
		
		// Write to log file
		pLog->Msg(ss.str().c_str());
	}
}

// Get the log file for writing
FILogFile* CvConnectionService::GetLogFile()
{
	return LOGFILEMGR.GetLog("connection.log", FILogFile::kDontTimeStamp);
}

// Named Pipe server thread function
DWORD WINAPI CvConnectionService::NamedPipeServerThread(LPVOID lpParam)
{
	CvConnectionService* pService = static_cast<CvConnectionService*>(lpParam);
	
	// Get pipe name from environment variable, default to "vox-deorum-bridge"
	char pipeNameBuffer[256];
	const char* envPipeName = getenv("VOX_DEORUM_PIPE_NAME");
	const char* basePipeName = envPipeName ? envPipeName : "vox-deorum-bridge";
	sprintf_s(pipeNameBuffer, sizeof(pipeNameBuffer), "\\\\.\\pipe\\tmp-app.%s", basePipeName);
	const char* pipeName = pipeNameBuffer;
	
	while (!pService->m_bShutdownRequested)
	{
		// Create the Named Pipe
		HANDLE hPipe = CreateNamedPipeA(
			pipeName,                  // pipe name
			PIPE_ACCESS_DUPLEX,        // read/write access
			PIPE_TYPE_MESSAGE |        // message type pipe
			PIPE_READMODE_MESSAGE |    // message-read mode
			PIPE_NOWAIT,               // non-blocking mode
			1,                         // max instances (only one client - Bridge Service)
			65536,                      // output buffer size
			65536,                      // input buffer size
			0,                         // client time-out
			NULL);                     // default security attributes
		
		if (hPipe == INVALID_HANDLE_VALUE)
		{
			std::stringstream ss;
			ss << "NamedPipeServerThread - Failed to create Named Pipe, error: " << GetLastError();
			pService->Log(LOG_ERROR, ss.str().c_str());
			Sleep(50); // Wait before retrying
			continue;
		}
		
		pService->m_hPipe = hPipe;
		pService->Log(LOG_INFO, "NamedPipeServerThread - Named Pipe created, waiting for client connection");
		
		// Wait for client to connect (non-blocking mode)
		BOOL waitingForClient = true;
		while (waitingForClient && !pService->m_bShutdownRequested)
		{
			BOOL connected = ConnectNamedPipe(hPipe, NULL);
			DWORD error = GetLastError();
			
			if (connected || error == ERROR_PIPE_CONNECTED)
			{
				// Client successfully connected
				waitingForClient = false;
			}
			else if (error == ERROR_PIPE_LISTENING)
			{
				// Still waiting for client connection in non-blocking mode
				Sleep(50); // Wait 50ms before checking again
			}
			else if (error == ERROR_NO_DATA)
			{
				// Pipe is in non-blocking mode and no client is ready
				Sleep(50); // Wait 50ms before checking again
			}
			else
			{
				// Actual error occurred
				std::stringstream ss;
				ss << "NamedPipeServerThread - Failed to connect to client, error: " << error;
				pService->Log(LOG_ERROR, ss.str().c_str());
				CloseHandle(hPipe);
				pService->m_hPipe = INVALID_HANDLE_VALUE;
				waitingForClient = false;
				
				break; // Exit the inner loop to create a new pipe
			}
		}
		
		// Check if we should continue (client connected) or retry (error or shutdown)
		if (pService->m_bShutdownRequested || pService->m_hPipe == INVALID_HANDLE_VALUE)
		{
			if (pService->m_hPipe != INVALID_HANDLE_VALUE)
			{
				CloseHandle(hPipe);
				pService->m_hPipe = INVALID_HANDLE_VALUE;
			}
			continue;
		}
		
		// Client connected
		pService->m_bClientConnected = true;
		pService->Log(LOG_INFO, "NamedPipeServerThread - Bridge Service connected");
		
		// Handle the client connection
		pService->HandleClientConnection(hPipe);
		
		// Client disconnected
		pService->m_bClientConnected = false;
		pService->Log(LOG_INFO, "NamedPipeServerThread - Bridge Service disconnected");
		
		// Cleanup
		if (pService->m_hPipe != INVALID_HANDLE_VALUE) {
			DisconnectNamedPipe(hPipe);
			CloseHandle(hPipe);
			pService->m_hPipe = INVALID_HANDLE_VALUE;
		}
		
		// Small delay before accepting new connection
		if (!pService->m_bShutdownRequested)
		{
			Sleep(50);
		}
	}
	
	pService->Log(LOG_INFO, "NamedPipeServerThread - Thread exiting");
	return 0;
}

// Handle a single client connection
void CvConnectionService::HandleClientConnection(HANDLE hPipe)
{
	// Dynamic message buffer to avoid repeated allocations
	std::string messageBuffer;
	messageBuffer.reserve(262144); // Reserve initial capacity
	
	// Temporary read buffer for pipe operations
	char readBuffer[262144];
	DWORD bytesRead, bytesWritten;
	
	while (m_bClientConnected && !m_bShutdownRequested)
	{
		// Send ALL queued outgoing messages
		bool hasOutgoingMessages = true;
		while (hasOutgoingMessages && m_bClientConnected && !m_bShutdownRequested)
		{
			std::string outgoingMessage;
			if (DequeueOutgoingMessage(outgoingMessage))
			{
				// Add delimiter to the message
				outgoingMessage += "!@#$%^!";
				
				// Send the message to the Bridge Service
				BOOL success = WriteFile(
					hPipe,
					outgoingMessage.c_str(),
					(DWORD)outgoingMessage.length(),
					&bytesWritten,
					NULL);
				
				if (!success)
				{
					DWORD error = GetLastError();
					
					// If pipe is busy, put the message back and break inner loop
					if (error == ERROR_PIPE_BUSY || error == ERROR_NO_DATA)
					{
						// Re-queue the message at the front (this is a limitation of std::queue)
						// For now, we'll just log and skip
						std::stringstream ss;
						ss << "HandleClientConnection - Pipe busy, will retry later. Error: " << error;
						Log(LOG_WARNING, ss.str().c_str());
						hasOutgoingMessages = false;
					}
					else
					{
						std::stringstream ss;
						ss << "HandleClientConnection - Failed to send outgoing message, error: " << error;
						Log(LOG_ERROR, ss.str().c_str());
						break; // Break from inner loop on serious error
					}
				}
				else
				{
					std::stringstream ss;
					ss << "HandleClientConnection - Sent outgoing message: " << outgoingMessage;
					Log(LOG_DEBUG, ss.str().c_str());
				}
			}
			else
			{
				// No more messages to send
				hasOutgoingMessages = false;
			}
		}
		
		// Flush the pipe after sending all messages to ensure the client receives the data
		if (m_bClientConnected && !m_bShutdownRequested)
		{
			FlushFileBuffers(hPipe);
		}
		
		// Read and process messages from client (non-blocking)
		bool hasIncomingData = true;
		int messagesProcessed = 0;
		const int maxMessagesPerIteration = 100; // Prevent processing too many messages at once
		const std::string delimiter = "!@#$%^!";
		
		while (hasIncomingData && messagesProcessed < maxMessagesPerIteration && m_bClientConnected && !m_bShutdownRequested)
		{
			// Try to read data from pipe
			BOOL success = ReadFile(
				hPipe,
				readBuffer,
				sizeof(readBuffer) - 1,
				&bytesRead,
				NULL);
			
			if (!success)
			{
				DWORD error = GetLastError();
				
				// If no data available, we've read everything
				if (error == ERROR_NO_DATA)
				{
					hasIncomingData = false;
				}
				// Handle actual errors
				else if (error == ERROR_BROKEN_PIPE || error == ERROR_PIPE_NOT_CONNECTED)
				{
					Log(LOG_INFO, "HandleClientConnection - Client disconnected");
					return; // Break from main loop
				}
				else
				{
					std::stringstream ss;
					ss << "HandleClientConnection - ReadFile failed, error: " << error;
					Log(LOG_ERROR, ss.str().c_str());
					return; // Break from main loop
				}
			}
			else if (bytesRead == 0)
			{
				// No data read, we've read everything
				hasIncomingData = false;
			}
			else
			{
				// Successfully read data
				// Null-terminate the received data
				readBuffer[bytesRead] = '\0';
				
				// Append to message buffer
				messageBuffer.append(readBuffer, bytesRead);
				
				// Process complete messages from the buffer immediately
				size_t pos = 0;
				while ((pos = messageBuffer.find(delimiter)) != std::string::npos)
				{
					// Extract the message
					std::string singleMessage = messageBuffer.substr(0, pos);
					
					// Only queue non-empty messages
					if (!singleMessage.empty())
					{
						// Lock the incoming queue critical section
						EnterCriticalSection(&m_csIncoming);
						m_incomingQueue.push(singleMessage);
						LeaveCriticalSection(&m_csIncoming);
						
						// Log the queuing action
						std::stringstream ss;
						ss << "HandleClientConnection - Queued message: " << singleMessage;
						Log(LOG_DEBUG, ss.str().c_str());
						
						messagesProcessed++;
					}
					
					// Remove the processed message and delimiter from buffer
					messageBuffer.erase(0, pos + delimiter.length());
					
					// Break if we've processed enough messages
					if (messagesProcessed >= maxMessagesPerIteration)
					{
						hasIncomingData = false; // Stop reading for this iteration
						Log(LOG_WARNING, "HandleClientConnection - Reached max messages per iteration limit");
						break;
					}
				}
				
				// Continue reading more data if we haven't hit the limit
			}
		}
		
		// Check if buffer is getting too large (prevent memory issues)
		if (messageBuffer.size() > 10485760) // 10MB limit for incomplete messages
		{
			std::stringstream ss;
			ss << "HandleClientConnection - Message buffer too large (" << messageBuffer.size() 
			   << " bytes), clearing to prevent memory issues";
			Log(LOG_WARNING, ss.str().c_str());
			messageBuffer.clear();
			messageBuffer.reserve(262144); // Re-reserve standard capacity
		}
		
		// Small delay only if we didn't process any messages
		if (messagesProcessed == 0 && !hasIncomingData && !hasOutgoingMessages)
		{
			Sleep(20); // Small delay to avoid busy waiting when idle
		}
	}
	
	// Log any remaining data in buffer when disconnecting
	if (!messageBuffer.empty())
	{
		std::stringstream ss;
		ss << "HandleClientConnection - Connection closed with " << messageBuffer.size() 
		   << " bytes of incomplete data in buffer";
		Log(LOG_WARNING, ss.str().c_str());
	}
}


// Queue an outgoing message to send to the Bridge Service (called from game thread)
// Returns the length of the outgoing queue after adding the message
int CvConnectionService::QueueOutgoingMessage(const std::string& messageJson)
{
	// Lock the outgoing queue critical section
	EnterCriticalSection(&m_csOutgoing);
	
	// Add the message to the queue
	m_outgoingQueue.push(messageJson);
	
	// Get the queue size
	int queueSize = static_cast<int>(m_outgoingQueue.size());
	
	// Release the critical section
	LeaveCriticalSection(&m_csOutgoing);
	
	return queueSize;
}

// Dequeue an outgoing message to send to the Bridge Service (called from SSE thread)
bool CvConnectionService::DequeueOutgoingMessage(std::string& messageJson)
{
	bool hasMessage = false;
	
	// Lock the outgoing queue critical section
	EnterCriticalSection(&m_csOutgoing);
	
	// Check if there are messages to send
	if (!m_outgoingQueue.empty())
	{
		// Get the message from the front of the queue
		messageJson = m_outgoingQueue.front();
		m_outgoingQueue.pop();
		hasMessage = true;
	}
	
	// Release the critical section
	LeaveCriticalSection(&m_csOutgoing);
	
	return hasMessage;
}

// Process queued messages from the main game thread
void CvConnectionService::ProcessMessages()
{
	// Only process if we're initialized
	if (!m_bInitialized)
	{
		return;
	}
	
	try {
		// Lock the incoming queue critical section
		EnterCriticalSection(&m_csIncoming);
		
		// Process all queued messages
		int processedCount = 0;
		while (!m_incomingQueue.empty())
		{
			// Get the message from the front of the queue
			std::string msgJson = m_incomingQueue.front();
			m_incomingQueue.pop();
			
			// Temporarily release the lock to process the message
			LeaveCriticalSection(&m_csIncoming);
			
			// Route the message to the appropriate handler
			RouteMessage(msgJson);
			
			processedCount++;
			
			// Re-acquire the lock for the next iteration
			EnterCriticalSection(&m_csIncoming);
		}
		
		// Release the critical section
		LeaveCriticalSection(&m_csIncoming);
	}
	catch (...)
	{
		std::stringstream ss;
		ss << "ProcessMessages - Unknown exception processing messages";
		Log(LOG_ERROR, ss.str().c_str());
	}
}

// Send a message to the Bridge Service (called from game code)
// Returns the length of the outgoing queue after adding the message
int CvConnectionService::SendMessage(const DynamicJsonDocument& message)
{
	if (!m_bInitialized)
	{
		Log(LOG_WARNING, "SendMessage - Cannot send message, service not initialized");
		return 0;
	}
	
	// Serialize the message
	std::string messageStr;
	serializeJson(message, messageStr);
	
	// Queue the wrapped message and return the queue size
	return QueueOutgoingMessage(messageStr);
}

// Route incoming message to appropriate handler based on type
void CvConnectionService::RouteMessage(const std::string& messageJson)
{
	// Clear and reuse the read buffer for main thread message processing
	m_pMainThreadReadBuffer->clear();
	DeserializationError error = deserializeJson(*m_pMainThreadReadBuffer, messageJson);
	
	if (error)
	{
		std::stringstream ss;
		ss << "RouteMessage - Failed to parse JSON: " << error.c_str();
		Log(LOG_ERROR, ss.str().c_str());
		return;
	}
	
	const char* messageType = (*m_pMainThreadReadBuffer)["type"];
	// Log(LOG_DEBUG, ("Routing message of type: " + std::string(messageType)).c_str());
	
	// Route to appropriate handler based on message type
	if (strcmp(messageType, "lua_execute") == 0)
	{
		// Extract parameters from JSON message
		const char* script = (*m_pMainThreadReadBuffer)["script"];
		const char* id = (*m_pMainThreadReadBuffer)["id"];
		
		// Call handler with extracted parameters
		if (script && id) HandleLuaExecute(script, id);
	}
	else if (strcmp(messageType, "lua_call") == 0)
	{
		// Extract parameters from JSON message
		const char* functionName = (*m_pMainThreadReadBuffer)["function"];
		const char* id = (*m_pMainThreadReadBuffer)["id"];
		JsonArray args = (*m_pMainThreadReadBuffer)["args"].as<JsonArray>();
		
		// Call handler with extracted parameters
		if (functionName && id) HandleLuaCall(functionName, args, id);
	}
	else if (strcmp(messageType, "external_register") == 0)
	{
		// Extract parameters for external function registration
		const char* functionName = (*m_pMainThreadReadBuffer)["name"];
		bool bAsync = (*m_pMainThreadReadBuffer)["async"] | false;
		
		// Register the external function
		if (functionName) RegisterExternalFunction(functionName, bAsync);
	}
	else if (strcmp(messageType, "external_unregister") == 0)
	{
		// Extract function name for unregistration
		const char* functionName = (*m_pMainThreadReadBuffer)["name"];
		
		// Unregister the external function
		if (functionName) UnregisterExternalFunction(functionName);
	}
	else if (strcmp(messageType, "external_response") == 0)
	{
		// Extract parameters for external call response
		const char* callId = (*m_pMainThreadReadBuffer)["id"];
		bool bSuccess = (*m_pMainThreadReadBuffer)["success"] | false;
		
		// Parse error code from error object if present
		const char* error = nullptr;
		if (m_pMainThreadReadBuffer->containsKey("error") && (*m_pMainThreadReadBuffer)["error"].containsKey("code"))
		{
			error = (*m_pMainThreadReadBuffer)["error"]["code"];
		}
		
		// Convert result to string if present (protocol uses "result" field)
		const char* data = nullptr;
		std::string dataStr;
		if (m_pMainThreadReadBuffer->containsKey("result"))
		{
			serializeJson((*m_pMainThreadReadBuffer)["result"], dataStr);
			data = dataStr.c_str();
		}
		
		// Handle the response
		if (callId) HandleExternalCallResponse(callId, bSuccess, error, data);
	}
	else
	{
		// Otherwise, create echo response with original message
		// Use the write buffer for the response
		m_pMainThreadWriteBuffer->clear();
		(*m_pMainThreadWriteBuffer)["type"] = "echo_response";
		(*m_pMainThreadWriteBuffer)["original"] = messageType;  // Include the unrecognized type
		(*m_pMainThreadWriteBuffer)["timestamp"] = GetTickCount();
		(*m_pMainThreadWriteBuffer)["turn"] = GC.getGame().getElapsedGameTurns();
		
		// Send the response back to the Bridge Service
		SendMessage(*m_pMainThreadWriteBuffer);
	}
}

// Handle Lua execute command from Bridge Service
void CvConnectionService::HandleLuaExecute(const char* script, const char* id)
{
	std::stringstream logMsg;
	logMsg << "HandleLuaExecute - Executing script for id: " << id;
	Log(LOG_DEBUG, logMsg.str().c_str());

	// Handle thread locks for safe Lua execution
	bool bHadLock = gDLL->HasGameCoreLock();
	if (bHadLock)
	{
		gDLL->ReleaseGameCoreLock();
	}

	// Execute the Lua script using luaL_dostring (which is luaL_loadstring + lua_pcall)
	int result = luaL_dostring(m_pLuaState, script);

	// Process the Lua execution result
	ProcessLuaResult(m_pLuaState, result, id);

	// Restore game core lock if we had it
	if (bHadLock)
	{
		gDLL->GetGameCoreLock();
	}
}

// Handle Lua function call from Bridge Service
void CvConnectionService::HandleLuaCall(const char* functionName, const JsonArray& args, const char* id)
{
	std::stringstream logMsg;
	logMsg << "HandleLuaCall - Calling function '" << functionName << "' for id: " << id;
	Log(LOG_DEBUG, logMsg.str().c_str());

	// Look up the registered function
	EnterCriticalSection(&m_csFunctions);
	
	std::map<std::string, LuaFunctionInfo>::iterator it = m_registeredFunctions.find(functionName);
	if (it == m_registeredFunctions.end())
	{
		// Function not found
		LeaveCriticalSection(&m_csFunctions);
		
		// Send error response using write buffer
		m_pMainThreadWriteBuffer->clear();
		(*m_pMainThreadWriteBuffer)["type"] = "lua_response";
		(*m_pMainThreadWriteBuffer)["id"] = id;
		(*m_pMainThreadWriteBuffer)["success"] = false;
		
		std::stringstream errorMsg;
		errorMsg << "Function '" << functionName << "' not found";
		(*m_pMainThreadWriteBuffer)["error"]["code"] = "INVALID_FUNCTION";
		(*m_pMainThreadWriteBuffer)["error"]["message"] = errorMsg.str();
		
		SendMessage(*m_pMainThreadWriteBuffer);
		
		logMsg.str("");
		logMsg << "HandleLuaCall - Function '" << functionName << "' not found";
		Log(LOG_WARNING, logMsg.str().c_str());
		return;
	}
	
	// Get the function info
	LuaFunctionInfo funcInfo = it->second;
	LeaveCriticalSection(&m_csFunctions);
	
	// Handle thread locks for safe Lua execution
	bool bHadLock = gDLL->HasGameCoreLock();
	if (bHadLock)
	{
		gDLL->ReleaseGameCoreLock();
	}
	
	// Push the function from the registry
	lua_rawgeti(funcInfo.pLuaState, LUA_REGISTRYINDEX, funcInfo.iRegistryRef);
	
	// Push arguments onto the Lua stack
	int argCount = 0;
	for (JsonArray::iterator argIt = args.begin(); argIt != args.end(); ++argIt)
	{
		ConvertJsonToLuaValue(funcInfo.pLuaState, *argIt);
		argCount++;
	}
	
	// Call the function (argCount args, LUA_MULTRET results)
	int result = lua_pcall(funcInfo.pLuaState, argCount, LUA_MULTRET, 0);
	
	// Process the result
	ProcessLuaResult(funcInfo.pLuaState, result, id);
	
	// Restore game core lock if we had it
	if (bHadLock)
	{
		gDLL->GetGameCoreLock();
	}
}

// Process the result of a Lua script execution
void CvConnectionService::ProcessLuaResult(lua_State* L, int executionResult, const char* id)
{
	// Build the response JSON using the write buffer
	m_pMainThreadWriteBuffer->clear();
	(*m_pMainThreadWriteBuffer)["type"] = "lua_response";
	(*m_pMainThreadWriteBuffer)["id"] = id;
	
	if (executionResult == 0)
	{
		// Script executed successfully
		(*m_pMainThreadWriteBuffer)["success"] = true;
		
		// Get the return value(s) from the stack
		int numResults = lua_gettop(L);
		
		if (numResults > 0)
		{
			// Use the shared conversion function (starting from stack bottom)
			ConvertLuaValuesInDocument(L, 1, numResults, *m_pMainThreadWriteBuffer, "result");
			
			// Pop all return values from the stack
			lua_pop(L, numResults);
		}

		std::stringstream logMsg;
		logMsg << "ProcessLuaResult - Sent success response for id: " << id << ", numResults: " << numResults;
		Log(LOG_DEBUG, logMsg.str().c_str());
	}
	else
	{
		// Script execution failed
		(*m_pMainThreadWriteBuffer)["success"] = false;
		
		const char* errorMsg = lua_tostring(L, -1);
		if (!errorMsg) errorMsg = "Unknown Lua error";
		
		// Clean up error message by stripping raw scripts
		std::string cleanedError = errorMsg;
		
		// Remove script location prefix (e.g., "summaries:" from "summaries:82:error message")
		size_t lastColon = cleanedError.rfind(':');
		if (lastColon != std::string::npos && lastColon > 0) {
			size_t secondLastColon = cleanedError.rfind(':', lastColon - 1);
			if (secondLastColon != std::string::npos) {
				// Keep everything after the second-to-last colon
				cleanedError = cleanedError.substr(secondLastColon + 1);
			}
		}
		
		(*m_pMainThreadWriteBuffer)["error"]["code"] = "LUA_EXECUTION_ERROR";
		(*m_pMainThreadWriteBuffer)["error"]["message"] = cleanedError;
		
		// Pop the error message from the stack
		lua_pop(L, 1);
		
		std::stringstream logMsg;
		logMsg << "ProcessLuaResult - Sent error response for id: " << id << ", error: " << errorMsg;
		Log(LOG_DEBUG, logMsg.str().c_str());
	}
	
	// Send the response
	SendMessage(*m_pMainThreadWriteBuffer);
}

// Shared function to convert Lua values to fill a key in a JsonDocument
void CvConnectionService::ConvertLuaValuesInDocument(lua_State* L, int firstIndex, int numValues, DynamicJsonDocument& parent, const char* key)
{
	if (numValues <= 0)
	{
		// No values - set to null
		parent[key] = nullptr;
	}
	else if (numValues == 1)
	{
		// Single value - convert directly
		ConvertLuaToJsonValue(L, firstIndex, parent.as<JsonObject>(), key);
	}
	else
	{
		// Multiple values - create an array
		JsonArray resultArray;
		resultArray = parent.createNestedArray(key);
		
		for (int i = 0; i < numValues; i++)
		{
			ConvertLuaToJsonValue(L, firstIndex + i, resultArray, nullptr);
		}
	}
}

// Convert a Lua value at the given stack index to a JsonVariant
// parent: The parent JsonVariant (object or array) that will contain this value
// key: The key/index to use when adding to parent (nullptr for direct assignment)
void CvConnectionService::ConvertLuaToJsonValue(lua_State* L, int index, JsonVariant parent, const char* key)
{
	// Make the index absolute if it's relative
	if (index < 0 && index > LUA_REGISTRYINDEX)
	{
		index = lua_gettop(L) + index + 1;
	}
	
	int type = lua_type(L, index);
	
	switch (type)
	{
	case LUA_TNIL:
		if (key && parent.is<JsonObject>())
		{
			parent[key] = nullptr;
		}
		else if (parent.is<JsonArray>())
		{
			parent.add(nullptr);
		}
		else
		{
			parent.set(nullptr);
		}
		break;
		
	case LUA_TBOOLEAN:
		{
			bool bValue = lua_toboolean(L, index) != 0;
			if (key && parent.is<JsonObject>())
			{
				parent[key] = bValue;
			}
			else if (parent.is<JsonArray>())
			{
				parent.add(bValue);
			}
			else
			{
				parent.set(bValue);
			}
		}
		break;
		
	case LUA_TNUMBER:
		{
			double num = lua_tonumber(L, index);
			// Check if it's an integer
			if (num == floor(num) && num >= INT_MIN && num <= INT_MAX)
			{
				int intNum = static_cast<int>(num);
				if (key && parent.is<JsonObject>())
				{
					parent[key] = intNum;
				}
				else if (parent.is<JsonArray>())
				{
					parent.add(intNum);
				}
				else
				{
					parent.set(intNum);
				}
			}
			else
			{
				if (key && parent.is<JsonObject>())
				{
					parent[key] = num;
				}
				else if (parent.is<JsonArray>())
				{
					parent.add(num);
				}
				else
				{
					parent.set(num);
				}
			}
		}
		break;
		
	case LUA_TSTRING:
		{
			const char* str = lua_tostring(L, index);
			if (key && parent.is<JsonObject>())
			{
				parent[key] = (str ? str : "");
			}
			else if (parent.is<JsonArray>())
			{
				parent.add(str ? str : "");
			}
			else
			{
				parent.set(str ? str : "");
			}
		}
		break;
		
	case LUA_TTABLE:
		{
			// Check if it's an array or object
			// A Lua table is considered a JSON array ONLY if:
			// 1. It has only numeric keys
			// 2. All keys are positive integers
			// 3. Keys are sequential from 1 to n with no gaps
			bool isArray = false;
			int maxIndex = 0;
			bool hasNonNumericKeys = false;
			int keyCount = 0;
			
			// First pass: check key types and find max index
			lua_pushnil(L);
			while (lua_next(L, index) != 0)
			{
				keyCount++;
				// Check key type
				if (lua_type(L, -2) == LUA_TNUMBER)
				{
					double keyNum = lua_tonumber(L, -2);
					
					// Check if it's a positive integer
					if (keyNum == floor(keyNum) && keyNum >= 1 && keyNum <= INT_MAX)
					{
						int keyInt = static_cast<int>(keyNum);
						if (keyInt > maxIndex) maxIndex = keyInt;
					}
				}
				else
				{
					// Non-numeric key found
					hasNonNumericKeys = true;
				}
				lua_pop(L, 1); // Pop value, keep key for next iteration
			}
			
			// Only consider it an array if:
			// - No non-numeric keys
			// - Key count matches max index (no gaps)
			// - Max index is positive
			if (!hasNonNumericKeys && maxIndex > 0 && keyCount == maxIndex)
			{
				// Final check: verify all indices from 1 to maxIndex exist
				isArray = true;
				for (int i = 1; i <= maxIndex; i++)
				{
					lua_pushnumber(L, i);
					lua_gettable(L, index);
					if (lua_isnil(L, -1))
					{
						isArray = false;
						lua_pop(L, 1);
						break;
					}
					lua_pop(L, 1);
				}
			}
			
			if (isArray && maxIndex > 0)
			{
				// Create JSON array
				JsonArray arr;
				if (key && parent.is<JsonObject>())
				{
					arr = parent.as<JsonObject>().createNestedArray(key);
				}
				else if (parent.is<JsonArray>())
				{
					arr = parent.as<JsonArray>().createNestedArray();
				}
				else
				{
					arr = parent.to<JsonArray>();
				}
				
				if (!arr)
				{
					Log(LOG_ERROR, "ConvertLuaToJsonValue - Failed to create JSON array!");
					if (key && parent.is<JsonObject>())
					{
						parent[key] = nullptr;
					}
					else if (parent.is<JsonArray>())
					{
						parent.add(nullptr);
					}
					else
					{
						parent.set(nullptr);
					}
					break;
				}
				
				// Add elements in order (1 to maxIndex)
				for (int i = 1; i <= maxIndex; i++)
				{
					lua_pushnumber(L, i);
					lua_gettable(L, index);
					
					// Convert directly into the array (nullptr key means add to array)
					ConvertLuaToJsonValue(L, -1, arr, nullptr);
					
					lua_pop(L, 1);
				}
			}
			else
			{
				// Create JSON object
				JsonObject obj;
				if (key && parent.is<JsonObject>())
				{
					obj = parent.as<JsonObject>().createNestedObject(key);
				}
				else if (parent.is<JsonArray>())
				{
					obj = parent.as<JsonArray>().createNestedObject();
				}
				else
				{
					obj = parent.to<JsonObject>();
				}
				
				if (!obj)
				{
					Log(LOG_ERROR, "ConvertLuaToJsonValue - Failed to create JSON object!");
					if (key && parent.is<JsonObject>())
					{
						parent[key] = nullptr;
					}
					else if (parent.is<JsonArray>())
					{
						parent.add(nullptr);
					}
					else
					{
						parent.set(nullptr);
					}
					break;
				}
				
				int objectKeyCount = 0;

				lua_pushnil(L);
				while (lua_next(L, index) != 0)
				{
					// Save the value index
					int valueIndex = lua_gettop(L);
					int keyIndex = valueIndex - 1;
					
					// Get key as string
					const char* key = nullptr;
					std::string keyStorage; // Storage for dynamically created keys (per iteration)
					
					if (lua_type(L, keyIndex) == LUA_TSTRING)
					{
						key = lua_tostring(L, keyIndex);
					}
					else if (lua_type(L, keyIndex) == LUA_TNUMBER)
					{
						// Convert number to string without modifying the stack
						double numKey = lua_tonumber(L, keyIndex);
						std::stringstream ss;
						if (numKey == floor(numKey))
						{
							ss << static_cast<int>(numKey);
						}
						else
						{
							ss << numKey;
						}
						keyStorage = ss.str();
						key = keyStorage.c_str();
					}
					else if (lua_type(L, keyIndex) == LUA_TBOOLEAN)
					{
						// Convert boolean to string
						key = lua_toboolean(L, keyIndex) ? "true" : "false";
					}
					else
					{
						// For other types, use type name as key
						std::stringstream ss;
						ss << "<" << lua_typename(L, lua_type(L, keyIndex)) << ">";
						keyStorage = ss.str();
						key = keyStorage.c_str();
					}
					
					if (key)
					{
						objectKeyCount++;
						// Add key-value pair to object
						// For dynamic keys, we need to store them in the JSON object first
						// to ensure the string remains valid during recursive calls
						if (!keyStorage.empty())
						{
							// Use the string directly with ArduinoJson to ensure it makes a copy
							obj[keyStorage] = JsonVariant();
							ConvertLuaToJsonValue(L, valueIndex, obj[keyStorage], nullptr);
						}
						else
						{
							// For string literals and lua strings, use the key directly
							ConvertLuaToJsonValue(L, valueIndex, obj, key);
						}
					}
					else
					{
						// Log warning about skipped key
						Log(LOG_WARNING, "ConvertLuaToJsonValue - Skipped table entry with null key");
					}
					
					lua_pop(L, 1); // Pop value, keep key for next iteration
				}
			}
		}
		break;
		
	case LUA_TFUNCTION:
	case LUA_TUSERDATA:
	case LUA_TTHREAD:
	case LUA_TLIGHTUSERDATA:
		{
			// These types cannot be serialized to JSON - store as string description
			std::stringstream ss;
			ss << "<" << lua_typename(L, type) << ">";
			if (key && parent.is<JsonObject>())
			{
				parent[key] = ss.str();
			}
			else if (parent.is<JsonArray>())
			{
				parent.add(ss.str());
			}
			else
			{
				parent.set(ss.str());
			}
		}
		break;
		
	default:
		if (key && parent.is<JsonObject>())
		{
			parent[key] = nullptr;
		}
		else if (parent.is<JsonArray>())
		{
			parent.add(nullptr);
		}
		else
		{
			parent.set(nullptr);
		}
		break;
	}
}

// Convert a JSON value to Lua and push it onto the stack
void CvConnectionService::ConvertJsonToLuaValue(lua_State* L, const JsonVariant& value)
{
	if (value.isNull())
	{
		lua_pushnil(L);
	}
	else if (value.is<bool>())
	{
		lua_pushboolean(L, value.as<bool>() ? 1 : 0);
	}
	else if (value.is<int>())
	{
		lua_pushnumber(L, value.as<int>());
	}
	else if (value.is<double>())
	{
		lua_pushnumber(L, value.as<double>());
	}
	else if (value.is<const char*>())
	{
		lua_pushstring(L, value.as<const char*>());
	}
	else if (value.is<JsonArray>())
	{
		JsonArray arr = value.as<JsonArray>();
		
		// Create a new table
		lua_newtable(L);
		
		// Add array elements
		int index = 1;
		for (JsonArray::iterator it = arr.begin(); it != arr.end(); ++it)
		{
			lua_pushnumber(L, index);
			ConvertJsonToLuaValue(L, *it);
			lua_settable(L, -3);
			index++;
		}
	}
	else if (value.is<JsonObject>())
	{
		JsonObject obj = value.as<JsonObject>();
		
		// Create a new table
		lua_newtable(L);
		
		// Add object properties
		for (JsonObject::iterator it = obj.begin(); it != obj.end(); ++it)
		{
			lua_pushstring(L, it->key().c_str());
			ConvertJsonToLuaValue(L, it->value());
			lua_settable(L, -3);
		}
	}
	else
	{
		// Unknown type, push nil
		lua_pushnil(L);
	}
}

//------------------------------------------------------------------------------
// Register a Lua function for external calling
void CvConnectionService::RegisterLuaFunction(const char* name, lua_State* L, int stackIndex)
{
	EnterCriticalSection(&m_csFunctions);
	
	// Check for existing registration and clear it
	std::map<std::string, LuaFunctionInfo>::iterator it = m_registeredFunctions.find(name);
	if (it != m_registeredFunctions.end())
	{
		// Clear the old reference from the Lua registry
		luaL_unref(it->second.pLuaState, LUA_REGISTRYINDEX, it->second.iRegistryRef);
		
		char buffer[512];
		sprintf_s(buffer, sizeof(buffer), "Overwriting existing registration for function: %s", name);
		Log(LOG_INFO, buffer);
	}
	
	// Create registry reference for the function
	lua_pushvalue(L, stackIndex);  // Push function to top of stack
	int ref = luaL_ref(L, LUA_REGISTRYINDEX);  // Store in registry and get reference
	
	// Store the function info
	LuaFunctionInfo info;
	info.pLuaState = L;
	info.iRegistryRef = ref;
	info.strDescription = "Registered from Lua";  // Could be enhanced with optional description parameter
	
	m_registeredFunctions[name] = info;
	LeaveCriticalSection(&m_csFunctions);
	
	// Send registration notification to Bridge
	DynamicJsonDocument message(512);
	message["type"] = "lua_register";
	message["function"] = name;
	message["description"] = info.strDescription.c_str();
	SendMessage(message);
	
	char buffer[512];
	sprintf_s(buffer, sizeof(buffer), "Registered Lua function: %s", name);
	Log(LOG_INFO, buffer);
}

//------------------------------------------------------------------------------
// Unregister a specific function
void CvConnectionService::UnregisterLuaFunction(const char* name)
{
	EnterCriticalSection(&m_csFunctions);
	
	std::map<std::string, LuaFunctionInfo>::iterator it = m_registeredFunctions.find(name);
	if (it != m_registeredFunctions.end())
	{
		// Clear the Lua registry reference
		luaL_unref(it->second.pLuaState, LUA_REGISTRYINDEX, it->second.iRegistryRef);
		
		// Remove from map
		m_registeredFunctions.erase(it);
		
		char buffer[512];
		sprintf_s(buffer, sizeof(buffer), "Unregistered Lua function: %s", name);
		Log(LOG_INFO, buffer);
	}
	
	LeaveCriticalSection(&m_csFunctions);
	
	// Send unregistration notification to Bridge
	DynamicJsonDocument message(512);
	message["type"] = "lua_unregister";
	message["function"] = name;
	SendMessage(message);
}\

//------------------------------------------------------------------------------
// Forward game events to the Bridge Service
void CvConnectionService::ForwardGameEvent(const char* eventName, ICvEngineScriptSystemArgs1* args)
{
	// Check if we're connected and ready to send
	if (!m_bInitialized || !m_bClientConnected)
	{
		return;
	}
	
	// Process messages from the Connection Service
	ProcessMessages();

	try
	{
		
		// Define blacklisted high-frequency events
		static const char* eventBlacklist[] = {
			"GameCoreUpdateBegin",
			"GameCoreUpdateEnd",
			"PlayerPreAIUnitUpdate",
			"BattleStarted",
			"BattleJoined",
			"BattleFinished",
			"GameCoreTestVictory",
			NULL  // Null terminator
		};
		
		// Check if this event is blacklisted
		for (int i = 0; eventBlacklist[i] != NULL; i++)
		{
			if (strcmp(eventName, eventBlacklist[i]) == 0)
			{
				return;  // Skip blacklisted events
			}
		}
		
		// Special handling for TileRevealed event - skip if from non-major civ
		if (strcmp(eventName, "TileRevealed") == 0 && args && args->Count() >= 6)
		{
			// TileRevealed parameters: (iPlotX, iPlotY, iTeam, iFromTeam, bFirst, iUnitOwner, iUnit)
			// The 6th parameter (index 5) is the unit owner (player ID)
			int iUnitOwner = -1;
			if (args->GetInt(5, iUnitOwner) && iUnitOwner >= 0 && iUnitOwner < MAX_CIV_PLAYERS)
			{
				CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iUnitOwner);
				if (!kPlayer.isMajorCiv())
				{
					// Skip TileRevealed events from non-major civs
					return;
				}
			}
		}
		
		// Generate event ID based on turn and sequence
		unsigned long long eventId = GenerateEventId();
		
		// Create the message document following the new protocol
		DynamicJsonDocument message(2048);
		message["id"] = eventId;
		message["type"] = "game_event";
		message["event"] = eventName;
		
		// Extract arguments to JSON payload
		JsonObject payload = message.createNestedObject("payload");
		
		if (args != NULL)
		{
			unsigned int argCount = args->Count();
			JsonArray argsArray = payload.createNestedArray("args");
			
			for (unsigned int i = 0; i < argCount; i++)
			{
				try
				{
					ICvEngineScriptSystemArgs1::ArgType argType = args->GetType(i);
					
					switch (argType)
					{
					case ICvEngineScriptSystemArgs1::ARGTYPE_BOOL:
						{
							bool bValue;
							if (args->GetBool(i, bValue))
							{
								argsArray.add(bValue);
							}
						}
						break;
						
					case ICvEngineScriptSystemArgs1::ARGTYPE_INT:
						{
							int iValue;
							if (args->GetInt(i, iValue))
							{
								argsArray.add(iValue);
							}
						}
						break;
						
					case ICvEngineScriptSystemArgs1::ARGTYPE_FLOAT:
						{
							float fValue;
							if (args->GetFloat(i, fValue))
							{
								argsArray.add(fValue);
							}
						}
						break;
						
					case ICvEngineScriptSystemArgs1::ARGTYPE_STRING:
						{
							char* szValue;
							if (args->GetString(i, szValue))
							{
								argsArray.add(szValue);
							}
						}
						break;
						
					case ICvEngineScriptSystemArgs1::ARGTYPE_NULL:
						argsArray.add((char*)NULL);
						break;
						
					default:
						// Unknown or ARGTYPE_NONE, skip
						break;
					}
				}
				catch (...)
				{
					std::stringstream ss;
					ss << "ForwardGameEvent - Unknown exception processing argument " << i << " for event '" << eventName << "'";
					Log(LOG_ERROR, ss.str().c_str());
				}
			}
		}
		
		// Send the message asynchronously via the queue
		if (SendMessage(message) >= 5) {
	 		Sleep(20); // Wait 20ms to throttle the game thread
			// This slows the game a bit, but shouldn't be a big issue other than observer mode - which is our intention
			// Also, even when the modmod is enabled, one has to connect to the Bridge Service
		}
	}
	catch (...)
	{
		std::stringstream ss;
		ss << "ForwardGameEvent - Unknown exception forwarding event '" << eventName << "'";
		Log(LOG_ERROR, ss.str().c_str());
	}
	
	// Process messages from the Connection Service
	ProcessMessages();
}

// Register an external function that can be called from Lua
void CvConnectionService::RegisterExternalFunction(const char* name, bool bAsync)
{
	if (!name || strlen(name) == 0)
	{
		Log(LOG_WARNING, "RegisterExternalFunction - Invalid function name");
		return;
	}

	EnterCriticalSection(&m_csExternalFunctions);
	
	// Check if already registered
	std::map<std::string, ExternalFunctionInfo>::iterator it = m_externalFunctions.find(name);
	if (it != m_externalFunctions.end())
	{
		// Update existing registration
		it->second.bAsync = bAsync;
		it->second.bRegistered = true;
		
		std::stringstream ss;
		ss << "RegisterExternalFunction - Updated existing registration for '" << name << "'";
		Log(LOG_DEBUG, ss.str().c_str());
	}
	else
	{
		// Add new registration
		ExternalFunctionInfo info;
		info.strName = name;
		info.bAsync = bAsync;
		info.bRegistered = true;
		
		m_externalFunctions[name] = info;
		
		std::stringstream ss;
		ss << "RegisterExternalFunction - Added new registration for '" << name << "'";
		Log(LOG_DEBUG, ss.str().c_str());
	}
	
	LeaveCriticalSection(&m_csExternalFunctions);
}

// Unregister an external function
void CvConnectionService::UnregisterExternalFunction(const char* name)
{
	if (!name || strlen(name) == 0)
	{
		Log(LOG_WARNING, "UnregisterExternalFunction - Invalid function name");
		return;
	}
	EnterCriticalSection(&m_csExternalFunctions);
	
	std::map<std::string, ExternalFunctionInfo>::iterator it = m_externalFunctions.find(name);
	if (it != m_externalFunctions.end())
	{
		// Remove the registration
		m_externalFunctions.erase(it);
		
		std::stringstream ss;
		ss << "UnregisterExternalFunction - Successfully unregistered '" << name << "'";
		Log(LOG_DEBUG, ss.str().c_str());
	}
	else
	{
		std::stringstream ss;
		ss << "UnregisterExternalFunction - Function '" << name << "' was not registered";
		Log(LOG_WARNING, ss.str().c_str());
	}
	
	LeaveCriticalSection(&m_csExternalFunctions);
}

// Check if an external function is registered
bool CvConnectionService::IsExternalFunctionRegistered(const char* name) const
{
	if (!name || strlen(name) == 0)
	{
		return false;
	}

	EnterCriticalSection(const_cast<CRITICAL_SECTION*>(&m_csExternalFunctions));
	
	bool bRegistered = false;
	std::map<std::string, ExternalFunctionInfo>::const_iterator it = m_externalFunctions.find(name);
	if (it != m_externalFunctions.end())
	{
		bRegistered = it->second.bRegistered;
	}
	
	LeaveCriticalSection(const_cast<CRITICAL_SECTION*>(&m_csExternalFunctions));
	
	return bRegistered;
}

// Generate a unique call ID for async external function calls
std::string CvConnectionService::GenerateCallId()
{
	// Use a combination of thread ID, tick count, and counter to ensure uniqueness
	std::stringstream ss;
	ss << "call_" << GetCurrentThreadId() << "_" << GetTickCount() << "_" << m_uiNextCallId++;
	return ss.str();
}

// Generate event ID based on turn and sequence
unsigned long long CvConnectionService::GenerateEventId()
{
	// Get current turn from the game
	unsigned int currentTurn = 0;
	currentTurn = GC.getGame().getGameTurn();
	
	// If turn has changed, reset event sequence
	if (currentTurn != m_uiCurrentTurn)
	{
		m_uiCurrentTurn = currentTurn;
		m_uiEventSequence = 1;
	}
	
	// Generate event ID: (turn * 1000000) + eventSequence
	unsigned long long eventId = (unsigned long long)m_uiCurrentTurn * 1000000ULL + m_uiEventSequence;
	
	// Increment sequence for next event
	m_uiEventSequence++;
	
	return eventId;
}

// Serialize m_uiEventSequence to database
void CvConnectionService::SerializeEventSequence()
{
	// Build Lua script to save event sequence
	std::stringstream script;
	script << "local saveDB = Modding.OpenSaveData();\n";
	script << "-- Create table if not exists\n";
	script << "for row in saveDB.Query('CREATE TABLE IF NOT EXISTS Deorum(\"ID\" INTEGER NOT NULL PRIMARY KEY, \"Key\" TEXT, \"Value\" TEXT)') do end\n";
	script << "-- Update or insert EventSequence value\n";
	script << "for row in saveDB.Query(\"DELETE FROM Deorum WHERE Key == 'EventSequence'\") do end\n";
	script << "for row in saveDB.Query(\"INSERT INTO Deorum (Key, Value) VALUES ('EventSequence', ?)\", tostring(" << m_uiEventSequence << ")) do end\n";
	
	// Execute the script
	int result = luaL_dostring(m_pLuaState, script.str().c_str());
	
	if (result != 0)
	{
		const char* errorMsg = lua_tostring(m_pLuaState, -1);
		std::stringstream ss;
		ss << "SerializeEventSequence - Failed to save: " << (errorMsg ? errorMsg : "unknown error");
		Log(LOG_ERROR, ss.str().c_str());
		lua_pop(m_pLuaState, 1);
	}
	else
	{
		std::stringstream ss;
		ss << "SerializeEventSequence - Saved event sequence: " << m_uiEventSequence;
		Log(LOG_DEBUG, ss.str().c_str());
	}
}

// Deserialize m_uiEventSequence from database
void CvConnectionService::DeserializeEventSequence()
{
	// Build Lua script to load event sequence
	const char* script = 
		"local saveDB = Modding.OpenSaveData();\n"
		"-- Create table if not exists\n"
		"for row in saveDB.Query('CREATE TABLE IF NOT EXISTS Deorum(\"ID\" INTEGER NOT NULL PRIMARY KEY, \"Key\" TEXT, \"Value\" TEXT)') do end\n"
		"-- Try to get existing EventSequence\n"
		"local eventSequence = nil;\n"
		"for row in saveDB.Query(\"SELECT Value FROM Deorum WHERE Key == 'EventSequence'\") do\n"
		"  eventSequence = row.Value;\n"
		"  break;\n"
		"end\n"
		"return eventSequence;\n";
	
	// Execute the script
	int result = luaL_dostring(m_pLuaState, script);
	
	if (result != 0)
	{
		const char* errorMsg = lua_tostring(m_pLuaState, -1);
		std::stringstream ss;
		ss << "DeserializeEventSequence - Failed to load: " << (errorMsg ? errorMsg : "unknown error");
		Log(LOG_ERROR, ss.str().c_str());
		lua_pop(m_pLuaState, 1);
	}
	else
	{
		// Check if we got a result
		if (lua_isstring(m_pLuaState, -1))
		{
			const char* valueStr = lua_tostring(m_pLuaState, -1);
			if (valueStr)
			{
				unsigned int loadedValue = (unsigned int)atoi(valueStr);
				if (loadedValue > 0)
				{
					m_uiEventSequence = loadedValue;
				}
			}
		}
		lua_pop(m_pLuaState, 1);
	}
	std::stringstream ss;
	ss << "DeserializeEventSequence - Loaded event sequence: " << m_uiEventSequence;
	Log(LOG_DEBUG, ss.str().c_str());
}

// Callback data structure for Lua external calls
struct LuaExternalCallbackData {
	lua_State* L;
	int callbackRef;
};

// Static callback function for external calls
void CvConnectionService::HandleExternalCallCallback(const ExternalCallResult& result, void* userData)
{
	LuaExternalCallbackData* pData = (LuaExternalCallbackData*)userData;
	if (pData && pData->L)
	{
		// Get the callback function from registry
		lua_rawgeti(pData->L, LUA_REGISTRYINDEX, pData->callbackRef);
		
		// Use shared post-processing
		CvConnectionService::GetInstance().ProcessExternalCallResult(pData->L, result);
		
		// Call the callback with 2 arguments (result/nil, error)
		if (lua_pcall(pData->L, 2, 0, 0) != 0)
		{
			// Log error but don't propagate
			const char* error = lua_tostring(pData->L, -1);
			std::stringstream logMsg;
			logMsg << "Error in external call callback: " << (error ? error : "unknown");
			CvConnectionService::GetInstance().Log(CvConnectionService::LOG_ERROR, logMsg.str().c_str());
			lua_pop(pData->L, 1);  // Remove error from stack
		}
		
		// Clean up the callback reference
		luaL_unref(pData->L, LUA_REGISTRYINDEX, pData->callbackRef);
		
		// Clean up the callback data
		delete pData;
	}
}

// Call an external function (callback extracted from Lua state if present)
int CvConnectionService::CallExternalFunction(lua_State* L)
{
	ExternalCallResult result;
	result.bSuccess = false;
	
	// Validate Lua state
	if (!L)
	{
		Log(LOG_ERROR, "CallExternalFunction - Invalid Lua state provided");
		return 0;
	}
	
	// Get function name from first argument
	if (!lua_isstring(L, 1))
	{
		Log(LOG_ERROR, "CallExternalFunction - First argument must be a function name string");
		lua_pushnil(L);
		lua_pushstring(L, "First argument must be a function name string");
		return 2;  // Pushed nil and error message
	}
	const char* functionName = lua_tostring(L, 1);
	
	// Check if last argument is a callback function
	int numArgs = lua_gettop(L);
	bool hasCallback = false;
	ExternalCallCallback callback = NULL;
	void* userData = NULL;
	
	// Check if last argument is a callback function (for async calls)
	if (numArgs >= 2 && lua_isfunction(L, numArgs))
	{
		hasCallback = true;
		
		// Create callback data
		LuaExternalCallbackData* pCallbackData = new LuaExternalCallbackData;
		pCallbackData->L = L;
		
		// Create a reference to the callback function
		lua_pushvalue(L, numArgs);  // Push callback to top
		pCallbackData->callbackRef = luaL_ref(L, LUA_REGISTRYINDEX);
		
		// Set the callback function pointer
		callback = HandleExternalCallCallback;
		userData = pCallbackData;
		
		// Don't include callback in arguments count
		numArgs--;
	}
	
	// Validate the call
	if (!ValidateExternalCall(functionName, result))
	{
		// For async calls, call the callback with an error immediately
		if (callback)
		{
			callback(result, userData);
			return 0;  // Async call, callback will handle the result
		}
		// For sync calls, push error to Lua stack
		else
		{
			lua_pushnil(L);
			lua_pushstring(L, result.strError.c_str());
			return 2;  // Pushed nil and error message
		}
	}
	
	// Generate unique call ID
	std::string callId = GenerateCallId();
	
	// Create a pending call entry
	PendingExternalCall pendingCall;
	pendingCall.strCallId = callId;
	pendingCall.strFunctionName = functionName;
	pendingCall.pCallback = callback;
	pendingCall.pUserData = userData;
	pendingCall.dwTimestamp = GetTickCount();
	
	// Add to pending calls map
	EnterCriticalSection(&m_csPendingCalls);
	m_pendingExternalCalls[callId] = pendingCall;
	LeaveCriticalSection(&m_csPendingCalls);
	
	// Create the call request message
	DynamicJsonDocument request(8192);
	request["type"] = "external_call";
	request["id"] = callId;
	request["function"] = functionName;
	request["sync"] = !hasCallback;
	
	// Convert Lua arguments directly to JSON without intermediate string serialization
	// Important: numArgs was already decremented if there was a callback, so it doesn't include the callback
	// Arguments start at index 2 (after function name at index 1)
	int actualArgCount = numArgs - 1;  // Exclude function name
	if (actualArgCount > 0)
	{
		// Use the shared conversion function to convert Lua values directly to JsonVariant
		ConvertLuaValuesInDocument(L, 2, actualArgCount, request, "args");
	}
	
	// Send the request
	SendMessage(request);
	
	std::stringstream logMsg;
	logMsg << "CallExternalFunction - Sent request for '" << functionName << "' with ID " << callId;
	Log(LOG_DEBUG, logMsg.str().c_str());
	
	// For synchronous calls, wait for the result
	if (!hasCallback)
	{
		// Wait for the result to be available in m_syncCallResults
		const DWORD maxWaitTime = 1000; // 1 second timeout
		const DWORD startTime = GetTickCount();
		ExternalCallResult syncResult;
		bool resultFound = false;
		
		while (!resultFound && (GetTickCount() - startTime) < maxWaitTime)
		{
			ProcessMessages(); // Process any incoming messages
			Sleep(10); // Wait 10ms before checking again

			EnterCriticalSection(&m_csPendingCalls);
			std::map<std::string, ExternalCallResult>::iterator resultIt = m_syncCallResults.find(callId);
			if (resultIt != m_syncCallResults.end())
			{
				syncResult = resultIt->second;
				m_syncCallResults.erase(resultIt); // Clean up the result
				resultFound = true;
			}
			LeaveCriticalSection(&m_csPendingCalls);
		}
		
		// Push result to Lua stack
		if (resultFound)
		{
			// Use shared post-processing which always pushes 2 values (result, error)
			ProcessExternalCallResult(L, syncResult);
			
			// Always return 2 (result and error/nil)
			return 2;
		}
		else
		{
			// Timeout occurred
			lua_pushnil(L);
			lua_pushstring(L, "CALL_TIMEOUT");
			
			// Clean up the pending call since it timed out
			EnterCriticalSection(&m_csPendingCalls);
			std::map<std::string, PendingExternalCall>::iterator pendingIt = m_pendingExternalCalls.find(callId);
			if (pendingIt != m_pendingExternalCalls.end())
			{
				m_pendingExternalCalls.erase(pendingIt);
			}
			LeaveCriticalSection(&m_csPendingCalls);
			
			std::stringstream timeoutMsg;
			timeoutMsg << "CallExternalFunction - Timeout waiting for response to '" << functionName << "' (ID: " << callId << ")";
			Log(LOG_WARNING, timeoutMsg.str().c_str());
			
			return 2;  // Pushed nil and error message
		}
	}
	else
	{
		// Async call - callback will handle the result later
		// IMPORTANT: We must clean up the Lua stack since we're returning 0
		// The original arguments are still on the stack and need to be removed
		// We have: function_name, arg1, arg2, ..., argN (callback was already removed from count)
		// Total items to pop: numArgs (which includes function name but excludes callback)
		lua_pop(L, numArgs);
	}
	
	// Async call - no values pushed (callback will handle the result)
	return 0;
}

// Handle external call response from the Bridge Service
void CvConnectionService::HandleExternalCallResponse(const char* callId, bool bSuccess, const char* error, const char* data)
{
	if (!callId)
	{
		Log(LOG_ERROR, "HandleExternalCallResponse - Invalid call ID");
		return;
	}
	
	// Look up the pending call
	EnterCriticalSection(&m_csPendingCalls);
	
	std::map<std::string, PendingExternalCall>::iterator it = m_pendingExternalCalls.find(callId);
	if (it == m_pendingExternalCalls.end())
	{
		LeaveCriticalSection(&m_csPendingCalls);
		
		std::stringstream ss;
		ss << "HandleExternalCallResponse - No pending call found for ID: " << callId;
		Log(LOG_WARNING, ss.str().c_str());
		return;
	}
	
	// Get the pending call info
	PendingExternalCall pendingCall = it->second;
	
	// Prepare the result
	ExternalCallResult result;
	result.bSuccess = bSuccess;
	result.strError = error ? error : "";
	result.strData = data ? data : "";
	
	// Log the response
	std::stringstream logMsg;
	logMsg << "HandleExternalCallResponse - Received response for '" << pendingCall.strFunctionName 
	       << "' (ID: " << callId << ", Success: " << (bSuccess ? "true" : "false") << ", Error: " << result.strError << ")";
	Log(LOG_DEBUG, logMsg.str().c_str());
	
	// Check if this is a sync or async call
	if (pendingCall.pCallback)
	{
		// Async call - remove from pending and invoke callback
		m_pendingExternalCalls.erase(it);
		LeaveCriticalSection(&m_csPendingCalls);
		
		// Call the callback outside of critical section
		pendingCall.pCallback(result, pendingCall.pUserData);
	}
	else
	{
		// Sync call - store the result for the waiting thread
		m_syncCallResults[callId] = result;
		m_pendingExternalCalls.erase(it);
		LeaveCriticalSection(&m_csPendingCalls);
	}
}

// Helper function to validate external call parameters and state
bool CvConnectionService::ValidateExternalCall(const char* functionName, ExternalCallResult& result)
{
	// Check if service is initialized and connected
	if (!m_bInitialized || !m_bClientConnected)
	{
		result.bSuccess = false;
		result.strError = "DLL_DISCONNECTED";
		Log(LOG_WARNING, "ValidateExternalCall - Service not connected");
		return false;
	}
	
	// Check if function is registered
	EnterCriticalSection(&m_csExternalFunctions);
	std::map<std::string, ExternalFunctionInfo>::iterator it = m_externalFunctions.find(functionName);
	bool bRegistered = (it != m_externalFunctions.end() && it->second.bRegistered);
	LeaveCriticalSection(&m_csExternalFunctions);
	
	if (!bRegistered)
	{
		std::stringstream ss;
		result.bSuccess = false;
		result.strError = "INVALID_FUNCTION";
		ss << "External function '" << functionName << "' is not registered";
		Log(LOG_WARNING, result.strError.c_str());
		return false;
	}
	
	return true;
}

// Shared post-processing function for external call results
// IMPORTANT: This function ALWAYS pushes exactly 2 values to the Lua stack:
// 1. Result value (or nil on error)
// 2. Error message (or nil on success)
void CvConnectionService::ProcessExternalCallResult(lua_State* L, const ExternalCallResult& result)
{
	if (result.bSuccess)
	{
		// Parse JSON result and push to Lua
		if (!result.strData.empty())
		{
			DynamicJsonDocument doc(8192);
			DeserializationError error = deserializeJson(doc, result.strData);
			
			if (!error)
			{
				// Convert JSON to Lua value - push result and nil (no error)
				ConvertJsonToLuaValue(L, doc.as<JsonVariant>());
				lua_pushnil(L);  // No error
			}
			else
			{
				// JSON parsing failed - push nil and error message
				lua_pushnil(L);
				lua_pushstring(L, "SERIALIZATION_ERROR");
				std::stringstream errorMsg;
				errorMsg << "Failed to parse JSON result: " << error.c_str();
			}
		}
		else
		{
			// No data, push nil result and nil error
			lua_pushnil(L);
			lua_pushnil(L);  // No error
		}
	}
	else
	{
		// Call failed, push nil and error message
		lua_pushnil(L);
		lua_pushstring(L, result.strError.c_str());
	}
}

