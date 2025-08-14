/*	-------------------------------------------------------------------------------------------------------
	© 2024 Vox Deorum Development Team. Connection Service for Bridge Communication.
	------------------------------------------------------------------------------------------------------- */

#include "CvGameCoreDLLPCH.h"
#include "CvConnectionService.h"
#include "CvGlobals.h"
#include "FireWorks/FILogFile.h"
#include <sstream>

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
	
	pService->Log(LOG_INFO, "NamedPipeServerThread - Thread started");
	
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
			PIPE_WAIT,                 // blocking mode
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
			Sleep(100); // Wait before retrying
			continue;
		}
		
		pService->m_hPipe = hPipe;
		pService->Log(LOG_INFO, "NamedPipeServerThread - Named Pipe created, waiting for client connection");
		
		// Wait for client to connect
		BOOL connected = ConnectNamedPipe(hPipe, NULL);
		
		if (!connected && GetLastError() != ERROR_PIPE_CONNECTED)
		{
			std::stringstream ss;
			ss << "NamedPipeServerThread - Failed to connect to client, error: " << GetLastError();
			pService->Log(LOG_ERROR, ss.str().c_str());
			CloseHandle(hPipe);
			pService->m_hPipe = INVALID_HANDLE_VALUE;
			
			if (!pService->m_bShutdownRequested)
			{
				Sleep(100); // Wait before retrying
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
			Sleep(100);
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
	
	// Set pipe to non-blocking mode for periodic shutdown checks
	DWORD pipeMode = PIPE_READMODE_MESSAGE | PIPE_NOWAIT;
	SetNamedPipeHandleState(hPipe, &pipeMode, NULL, NULL);
	
	while (m_bClientConnected && !m_bShutdownRequested)
	{
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
		
		// Log the received message
		std::stringstream ss;
		ss << "HandleClientConnection - Received message (" << bytesRead << " bytes): " << buffer;
		Log(LOG_INFO, ss.str().c_str());
		
		// Echo the message back to the client
		success = WriteFile(
			hPipe,
			buffer,
			bytesRead,
			&bytesWritten,
			NULL);
		
		if (!success)
		{
			std::stringstream ss;
			ss << "HandleClientConnection - WriteFile failed, error: " << GetLastError();
			Log(LOG_ERROR, ss.str().c_str());
			break;
		}
		
		// Flush the pipe to ensure the client receives the data
		FlushFileBuffers(hPipe);
		
		std::stringstream ss2;
		ss2 << "HandleClientConnection - Echoed message back (" << bytesWritten << " bytes)";
		Log(LOG_DEBUG, ss2.str().c_str());
	}
}