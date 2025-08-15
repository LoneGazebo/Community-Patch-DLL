/*	-------------------------------------------------------------------------------------------------------
	© 2024 Vox Deorum Development Team. Connection Service for Bridge Communication.
	------------------------------------------------------------------------------------------------------- */

#include "CvGameCoreDLLPCH.h"
#include "CvConnectionService.h"
#include "CvGlobals.h"
#include "FireWorks/FILogFile.h"
#include <sstream>
#include <iomanip>

// Singleton instance getter
CvConnectionService& CvConnectionService::GetInstance()
{
	static CvConnectionService instance;
	return instance;
}

// Constructor
CvConnectionService::CvConnectionService()
	: m_bInitialized(false)
	, m_hPipe(INVALID_HANDLE_VALUE)
	, m_hThread(NULL)
	, m_dwThreadId(0)
	, m_bClientConnected(false)
	, m_bShutdownRequested(false)
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

	// Reset shutdown flag
	m_bShutdownRequested = false;
	m_bClientConnected = false;

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

	// Clean up critical sections
	DeleteCriticalSection(&m_csIncoming);
	DeleteCriticalSection(&m_csOutgoing);

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
			ss << "HandleClientConnection - Sent outgoing message (" << bytesWritten << " bytes)";
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

		// Strip the wrapper: {"type":"message","data":<actual_json>}
		buffer[bytesRead - 1] = '\0'; // Remove the trailing }
		char* dataStart = buffer + 25;
		char* dataEnd = buffer + bytesRead - 2;
		std::string strippedJson(dataStart, dataEnd);
		
		// Queue the incoming message
		QueueIncomingMessage(strippedJson);
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
		
		// Log the dequeuing action
		std::stringstream ss;
		ss << "DequeueOutgoingMessage - Dequeued message, remaining: " << m_outgoingQueue.size();
		Log(LOG_DEBUG, ss.str().c_str());
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
	
	// Wrap the message in the expected format
	std::string wrappedMessage = "{\"type\":\"message\",\"data\":" + messageStr + "}";
	
	// Queue the wrapped message
	QueueOutgoingMessage(wrappedMessage);
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
	
	// Otherwise, create echo response with original message
	DynamicJsonDocument response(8192);
	response["type"] = "echo_response";
	response["original"] = message;
	response["timestamp"] = GetTickCount();
	response["turn"] = GC.getGame().getElapsedGameTurns();
	
	// Send the response back to the Bridge Service
	SendMessage(response);
}