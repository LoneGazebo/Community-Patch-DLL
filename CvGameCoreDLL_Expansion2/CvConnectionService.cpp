/*	-------------------------------------------------------------------------------------------------------
	© 2024 Vox Deorum Development Team. Connection Service for Bridge Communication.
	------------------------------------------------------------------------------------------------------- */

#include "CvGameCoreDLLPCH.h"
#include "CvConnectionService.h"
#include "CvGlobals.h"
#include "CvGame.h"
#include "FireWorks/FILogFile.h"
#include "ICvDLLScriptSystem.h"
#include "Lua/CvLuaSupport.h"
#include <sstream>
#include <iomanip>
#include <cmath>
#include <climits>

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

	// Register game data with the Lua state
	LuaSupport::RegisterScriptData(m_pLuaState);
	Log(LOG_INFO, "ConnectionService::Setup() - Lua state initialized");

	// Create the Named Pipe server thread
	Log(LOG_INFO, "ConnectionService::Setup() - Creating Named Pipe server thread");
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
	ClearLuaFunctions();
	
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
	
	// Define the pipe name (matching what Bridge Service expects)
	const char* pipeName = "\\\\.\\pipe\\tmp-app.vox-deorum-bridge";
	
	while (!pService->m_bShutdownRequested)
	{
		// Create the Named Pipe
		pService->Log(LOG_INFO, "NamedPipeServerThread - Creating Named Pipe");
		
		HANDLE hPipe = CreateNamedPipeA(
			pipeName,                  // pipe name
			PIPE_ACCESS_DUPLEX,        // read/write access
			PIPE_TYPE_MESSAGE |        // message type pipe
			PIPE_READMODE_MESSAGE |    // message-read mode
			PIPE_NOWAIT,               // non-blocking mode
			1,                         // max instances (only one client - Bridge Service)
			4096,                      // output buffer size
			4096,                      // input buffer size
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
		DisconnectNamedPipe(hPipe);
		CloseHandle(hPipe);
		pService->m_hPipe = INVALID_HANDLE_VALUE;
		
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
	char buffer[4096];
	DWORD bytesRead, bytesWritten;
	
	while (m_bClientConnected && !m_bShutdownRequested)
	{
		// First, check for any outgoing messages to send
		std::string outgoingMessage;
		if (DequeueOutgoingMessage(outgoingMessage))
		{
			// Message is already in wrapped format
			// Send the message to the Bridge Service
			BOOL success = WriteFile(
				hPipe,
				outgoingMessage.c_str(),
				(DWORD)outgoingMessage.length(),
				&bytesWritten,
				NULL);
			
			if (!success)
			{
				std::stringstream ss;
				ss << "HandleClientConnection - Failed to send outgoing message, error: " << GetLastError();
				Log(LOG_ERROR, ss.str().c_str());
				break;
			}
			
			// Flush the pipe to ensure the client receives the data
			FlushFileBuffers(hPipe);
			
			std::stringstream ss;
			ss << "HandleClientConnection - Sent outgoing message: " << outgoingMessage;
			Log(LOG_DEBUG, ss.str().c_str());
		}
		
		// Read message from client (non-blocking)
		BOOL success = ReadFile(
			hPipe,
			buffer,
			sizeof(buffer) - 1,
			&bytesRead,
			NULL);
		
		if (!success)
		{
			DWORD error = GetLastError();
			
			// If no data available, wait a bit and check for shutdown
			if (error == ERROR_NO_DATA)
			{
				Sleep(50); // Small delay to avoid busy waiting
				continue;
			}
			
			// Handle actual errors
			if (error == ERROR_BROKEN_PIPE || error == ERROR_PIPE_NOT_CONNECTED)
			{
				Log(LOG_INFO, "HandleClientConnection - Client disconnected");
			}
			else
			{
				std::stringstream ss;
				ss << "HandleClientConnection - ReadFile failed, error: " << error;
				Log(LOG_ERROR, ss.str().c_str());
			}
			break;
		}
		
		if (bytesRead == 0)
		{
			// No data read, continue checking
			Sleep(50);
			continue;
		}
		
		// Null-terminate the received message
		buffer[bytesRead] = '\0';

		// Queue the incoming message
		std::string json(buffer);
		QueueIncomingMessage(json);
	}
}

// Queue an incoming message from the Bridge Service (called from SSE thread)
void CvConnectionService::QueueIncomingMessage(const std::string& messageJson)
{
	// Lock the incoming queue critical section
	EnterCriticalSection(&m_csIncoming);
	
	// Add the message to the queue
	m_incomingQueue.push(messageJson);
	
	// Log the queuing action
	std::stringstream ss;
	ss << "QueueIncomingMessage - " << messageJson;
	Log(LOG_DEBUG, ss.str().c_str());
	
	// Release the critical section
	LeaveCriticalSection(&m_csIncoming);
}

// Queue an outgoing message to send to the Bridge Service (called from game thread)
void CvConnectionService::QueueOutgoingMessage(const std::string& messageJson)
{
	// Lock the outgoing queue critical section
	EnterCriticalSection(&m_csOutgoing);
	
	// Add the message to the queue
	m_outgoingQueue.push(messageJson);
	
	// Log the queuing action
	std::stringstream ss;
	ss << "QueueOutgoingMessage - " << messageJson;
	Log(LOG_DEBUG, ss.str().c_str());
	
	// Release the critical section
	LeaveCriticalSection(&m_csOutgoing);
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

// Send a message to the Bridge Service (called from game code)
void CvConnectionService::SendMessage(const DynamicJsonDocument& message)
{
	if (!m_bInitialized || !m_bClientConnected)
	{
		Log(LOG_WARNING, "SendMessage - Cannot send message, service not connected");
		return;
	}
	
	// Serialize the message
	std::string messageStr;
	serializeJson(message, messageStr);
	
	// Queue the wrapped message
	QueueOutgoingMessage(messageStr);
}

// Route incoming message to appropriate handler based on type
void CvConnectionService::RouteMessage(const std::string& messageJson)
{
	// Parse the JSON string to process it
	DynamicJsonDocument message(8192);
	DeserializationError error = deserializeJson(message, messageJson);
	
	if (error)
	{
		std::stringstream ss;
		ss << "RouteMessage - Failed to parse JSON: " << error.c_str();
		Log(LOG_ERROR, ss.str().c_str());
		return;
	}
	
	const char* messageType = message["type"];
	Log(LOG_DEBUG, ("Routing message of type: " + std::string(messageType)).c_str());
	
	// Route to appropriate handler based on message type
	if (strcmp(messageType, "lua_execute") == 0)
	{
		// Extract parameters from JSON message
		const char* script = message["script"];
		const char* id = message["id"];
		
		// Call handler with extracted parameters
		if (script && id) HandleLuaExecute(script, id);
	}
	else if (strcmp(messageType, "lua_call") == 0)
	{
		// Extract parameters from JSON message
		const char* functionName = message["function"];
		const char* id = message["id"];
		JsonArray args = message["args"].as<JsonArray>();
		
		// Call handler with extracted parameters
		if (functionName && id) HandleLuaCall(functionName, args, id);
	}
	else if (strcmp(messageType, "external_register") == 0)
	{
		// Extract parameters for external function registration
		const char* functionName = message["name"];
		bool bAsync = message["async"] | false;
		
		// Register the external function
		if (functionName) RegisterExternalFunction(functionName, bAsync);
	}
	else if (strcmp(messageType, "external_unregister") == 0)
	{
		// Extract function name for unregistration
		const char* functionName = message["name"];
		
		// Unregister the external function
		if (functionName) UnregisterExternalFunction(functionName);
	}
	else if (strcmp(messageType, "external_response") == 0)
	{
		// Extract parameters for external call response
		const char* callId = message["id"];
		bool bSuccess = message["success"] | false;
		const char* error = message["error"];
		const char* data = nullptr;
		
		// Convert result to string if present (protocol uses "result" field)
		std::string dataStr;
		if (message.containsKey("result"))
		{
			serializeJson(message["result"], dataStr);
			data = dataStr.c_str();
		}
		
		// Handle the response
		if (callId) HandleExternalCallResponse(callId, bSuccess, error, data);
	}
	else
	{
		// Otherwise, create echo response with original message
		DynamicJsonDocument response(8192);
		response["type"] = "echo_response";
		response["original"] = message;
		response["timestamp"] = GetTickCount();
		response["turn"] = GC.getGame().getElapsedGameTurns();
		
		// Send the response back to the Bridge Service
		SendMessage(response);
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
		
		// Send error response
		DynamicJsonDocument response(512);
		response["type"] = "lua_response";
		response["id"] = id;
		response["success"] = false;
		
		std::stringstream errorMsg;
		errorMsg << "Function '" << functionName << "' not found";
		response["error"] = errorMsg.str();
		
		SendMessage(response);
		
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
	// Build the response JSON
	DynamicJsonDocument response(8192);
	response["type"] = "lua_response";
	response["id"] = id;
	
	if (executionResult == 0)
	{
		// Script executed successfully
		response["success"] = true;
		
		// Get the return value(s) from the stack
		int numResults = lua_gettop(L);
		
		if (numResults > 0)
		{
			// Use the shared conversion function (starting from stack bottom)
			ConvertLuaValuesToJsonVariant(L, 1, numResults, response["result"].to<JsonVariant>());
			
			// Pop all return values from the stack
			lua_pop(L, numResults);
		}

		std::stringstream logMsg;
		logMsg << "ProcessLuaResult - Sent success response for id: " << id;
		Log(LOG_DEBUG, logMsg.str().c_str());
	}
	else
	{
		// Script execution failed
		response["success"] = false;
		
		const char* errorMsg = lua_tostring(L, -1);
		if (!errorMsg) errorMsg = "Unknown Lua error";
		
		response["error"] = errorMsg;
		
		// Pop the error message from the stack
		lua_pop(L, 1);
		
		std::stringstream logMsg;
		logMsg << "ProcessLuaResult - Sent error response for id: " << id << ", error: " << errorMsg;
		Log(LOG_DEBUG, logMsg.str().c_str());
	}
	
	// Send the response
	SendMessage(response);
}

// Shared function to convert Lua values to JsonVariant (single or array)
void CvConnectionService::ConvertLuaValuesToJsonVariant(lua_State* L, int firstIndex, int numValues, JsonVariant dest)
{
	if (numValues <= 0)
	{
		// No values - set to null
		dest.set(nullptr);
	}
	else if (numValues == 1)
	{
		// Single value - convert directly
		ConvertLuaToJsonValue(L, firstIndex, dest);
	}
	else
	{
		// Multiple values - create an array
		JsonArray resultArray = dest.to<JsonArray>();
		for (int i = 0; i < numValues; i++)
		{
			ConvertLuaToJsonValue(L, firstIndex + i, resultArray.add());
		}
	}
}

// Convert a Lua value at the given stack index to a JsonVariant
void CvConnectionService::ConvertLuaToJsonValue(lua_State* L, int index, JsonVariant dest)
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
		dest.set(nullptr);
		break;
		
	case LUA_TBOOLEAN:
		dest.set(lua_toboolean(L, index) != 0);
		break;
		
	case LUA_TNUMBER:
		{
			double num = lua_tonumber(L, index);
			// Check if it's an integer
			if (num == floor(num) && num >= INT_MIN && num <= INT_MAX)
			{
				dest.set(static_cast<int>(num));
			}
			else
			{
				dest.set(num);
			}
		}
		break;
		
	case LUA_TSTRING:
		{
			const char* str = lua_tostring(L, index);
			dest.set(str ? str : "");
		}
		break;
		
	case LUA_TTABLE:
		{
			// Check if it's an array or object
			bool isArray = true;
			int expectedIndex = 1;
			
			// First pass: check if it's an array
			lua_pushnil(L);
			while (lua_next(L, index) != 0)
			{
				// Check if key is a number and matches expected index
				if (lua_type(L, -2) != LUA_TNUMBER || lua_tonumber(L, -2) != expectedIndex)
				{
					isArray = false;
				}
				lua_pop(L, 1);
				expectedIndex++;
			}
			
			if (isArray)
			{
				// Create JSON array
				JsonArray arr = dest.to<JsonArray>();
				lua_pushnil(L);
				while (lua_next(L, index) != 0)
				{
					// Add value to array
					ConvertLuaToJsonValue(L, -1, arr.add());
					lua_pop(L, 1);
				}
			}
			else
			{
				// Create JSON object
				JsonObject obj = dest.to<JsonObject>();
				lua_pushnil(L);
				while (lua_next(L, index) != 0)
				{
					// Get key as string
					const char* key = nullptr;
					if (lua_type(L, -2) == LUA_TSTRING)
					{
						key = lua_tostring(L, -2);
					}
					else
					{
						// Convert non-string key to string
						lua_pushvalue(L, -2);
						key = lua_tostring(L, -1);
						lua_pop(L, 1);
					}
					
					if (key)
					{
						// Add key-value pair to object
						ConvertLuaToJsonValue(L, -1, obj[key]);
					}
					
					lua_pop(L, 1);
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
			dest.set(ss.str());
		}
		break;
		
	default:
		dest.set(nullptr);
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
}

//------------------------------------------------------------------------------
// Clear all registered functions (called on shutdown)
void CvConnectionService::ClearLuaFunctions()
{
	EnterCriticalSection(&m_csFunctions);
	
	// Clear all Lua registry references
	for (std::map<std::string, LuaFunctionInfo>::iterator it = m_registeredFunctions.begin(); 
	     it != m_registeredFunctions.end(); ++it)
	{
		luaL_unref(it->second.pLuaState, LUA_REGISTRYINDEX, it->second.iRegistryRef);
	}
	
	// Clear the map
	m_registeredFunctions.clear();
	
	Log(LOG_INFO, "Cleared all registered Lua functions");
	
	LeaveCriticalSection(&m_csFunctions);
	
	// Send clear notification to Bridge (only if connected)
	if (m_bInitialized && m_bClientConnected)
	{
		DynamicJsonDocument message(512);
		message["type"] = "lua_clear";
		SendMessage(message);
	}
}

//------------------------------------------------------------------------------
// Forward game events to the Bridge Service
void CvConnectionService::ForwardGameEvent(const char* eventName, ICvEngineScriptSystemArgs1* args)
{
	try
	{
		// Check if we're connected and ready to send
		if (!m_bInitialized || !m_bClientConnected)
		{
			return;
		}
		
		// Define blacklisted high-frequency events
		static const char* eventBlacklist[] = {
			"GameCoreUpdateBegin",
			"GameCoreUpdateEnd",
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
		
		// Create the message document (2KB should be enough for most events)
		DynamicJsonDocument message(2048);
		message["type"] = "game_event";
		message["event"] = eventName;
		
		// Add timestamp (using game tick for now)
		CvGame& game = GC.getGame();
		message["timestamp"] = game.getGameTurn();
		
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
		SendMessage(message);
	}
	catch (...)
	{
		std::stringstream ss;
		ss << "ForwardGameEvent - Unknown exception forwarding event '" << eventName << "'";
		Log(LOG_ERROR, ss.str().c_str());
	}
}

// Register an external function that can be called from Lua
void CvConnectionService::RegisterExternalFunction(const char* name, bool bAsync)
{
	if (!name || strlen(name) == 0)
	{
		Log(LOG_WARNING, "RegisterExternalFunction - Invalid function name");
		return;
	}

	std::stringstream logMsg;
	logMsg << "RegisterExternalFunction - Registering external function '" << name << "' (async: " << (bAsync ? "true" : "false") << ")";
	Log(LOG_INFO, logMsg.str().c_str());

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

	std::stringstream logMsg;
	logMsg << "UnregisterExternalFunction - Unregistering external function '" << name << "'";
	Log(LOG_INFO, logMsg.str().c_str());

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

// Callback data structure for Lua external calls
struct LuaExternalCallbackData {
	lua_State* L;
	int callbackRef;
};

// Static callback function for external calls
void HandleExternalCallCallback(const CvConnectionService::ExternalCallResult& result, void* userData)
{
	LuaExternalCallbackData* pData = (LuaExternalCallbackData*)userData;
	if (pData && pData->L)
	{
		// Get the callback function from registry
		lua_rawgeti(pData->L, LUA_REGISTRYINDEX, pData->callbackRef);
		
		// Push success status
		lua_pushboolean(pData->L, result.bSuccess);
		
		// Push result or error
		if (result.bSuccess)
		{
			// Parse JSON result and push to Lua
			if (!result.strData.empty())
			{
				DynamicJsonDocument doc(8192);
				DeserializationError error = deserializeJson(doc, result.strData);
				
				if (!error)
				{
					// Convert JSON to Lua value
					CvConnectionService::GetInstance().ConvertJsonToLuaValue(pData->L, doc.as<JsonVariant>());
				}
				else
				{
					// Push nil on parse error
					lua_pushnil(pData->L);
				}
			}
			else
			{
				// No data, push nil
				lua_pushnil(pData->L);
			}
		}
		else
		{
			// Push error message
			lua_pushstring(pData->L, result.strError.c_str());
		}
		
		// Call the callback with 2 arguments (success, result/error)
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
void CvConnectionService::CallExternalFunction(const char* functionName, lua_State* L, int firstArg)
{
	ExternalCallResult result;
	result.bSuccess = false;
	
	// Validate Lua state
	if (!L)
	{
		Log(LOG_ERROR, "CallExternalFunction - Invalid Lua state provided");
		return;
	}
	
	// Check if last argument is a callback function
	int numArgs = lua_gettop(L);
	bool hasCallback = false;
	ExternalCallCallback callback = NULL;
	void* userData = NULL;
	
	// Check if last argument is a callback function (for async calls)
	if (numArgs >= firstArg && lua_isfunction(L, numArgs))
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
		// Call the callback with an error immediately
		if (callback)
		{
			callback(result, userData);
		}
		return;
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
	SetupExternalCallRequest(request, callId.c_str(), functionName, L, firstArg, false);
	
	// Send the request
	SendMessage(request);
	
	std::stringstream logMsg;
	logMsg << "CallExternalFunction - Sent request for '" << functionName << "' with ID " << callId;
	Log(LOG_DEBUG, logMsg.str().c_str());
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
	       << "' (ID: " << callId << ", Success: " << (bSuccess ? "true" : "false") << ")";
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
		result.strError = "Connection service not initialized or not connected";
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
		ss << "External function '" << functionName << "' is not registered";
		result.bSuccess = false;
		result.strError = ss.str();
		Log(LOG_WARNING, result.strError.c_str());
		return false;
	}
	
	return true;
}

// Helper function to setup external call request JSON
void CvConnectionService::SetupExternalCallRequest(DynamicJsonDocument& request, const char* callId, const char* functionName, 
                                                   lua_State* L, int firstArg, bool isSync)
{
	request["type"] = "external_call";
	request["id"] = callId;
	request["function"] = functionName;
	request["sync"] = isSync;
	
	// Convert Lua arguments directly to JSON without intermediate string serialization
	if (L)
	{
		int numArgs = lua_gettop(L) - firstArg + 1;
		
		// Use the shared conversion function to convert Lua values directly to JsonVariant
		ConvertLuaValuesToJsonVariant(L, firstArg, numArgs, request["args"]);
	}
	else
	{
		// No Lua state provided, set args as null
		request["args"] = nullptr;
	}
}

