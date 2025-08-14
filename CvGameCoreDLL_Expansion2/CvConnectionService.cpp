/*	-------------------------------------------------------------------------------------------------------
	© 2024 Vox Deorum Development Team. Connection Service for Bridge Communication.
	------------------------------------------------------------------------------------------------------- */

#include "CvGameCoreDLLPCH.h"
#include "CvConnectionService.h"
#include "CvGlobals.h"
#include "FireWorks/FILogFile.h"
#include <ws2tcpip.h>
#include <sstream>

#pragma comment(lib, "ws2_32.lib")

// Singleton instance getter
CvConnectionService& CvConnectionService::GetInstance()
{
	static CvConnectionService instance;
	return instance;
}

// Constructor
CvConnectionService::CvConnectionService()
	: m_bInitialized(false)
	, m_socket(INVALID_SOCKET)
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

	// Initialize Winsock
	WSADATA wsaData;
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0)
	{
		std::stringstream ss;
		ss << "ConnectionService::Setup() - WSAStartup failed with error: " << result;
		Log(LOG_ERROR, ss.str().c_str());
		return false;
	}

	m_bInitialized = true;
	Log(LOG_INFO, "ConnectionService::Setup() - Successfully initialized");
	
	// Test logging at different levels
	Log(LOG_DEBUG, "ConnectionService::Setup() - Debug test message");
	Log(LOG_INFO, "ConnectionService::Setup() - Info test message");
	Log(LOG_WARNING, "ConnectionService::Setup() - Warning test message (this is just a test)");
	
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

	// Close socket if open
	if (m_socket != INVALID_SOCKET)
	{
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;
	}

	// Cleanup Winsock
	WSACleanup();

	m_bInitialized = false;
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