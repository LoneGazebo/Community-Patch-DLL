/*	-------------------------------------------------------------------------------------------------------
	© 2024 Vox Deorum Development Team. Connection Service for Bridge Communication.
	------------------------------------------------------------------------------------------------------- */
#pragma once

#ifndef CIV5_CONNECTION_SERVICE_H
#define CIV5_CONNECTION_SERVICE_H

#include <windows.h>
#include <string>

// Forward declarations
class FILogFile;

// Connection Service for communicating with external Bridge Service
class CvConnectionService
{
public:
	// Singleton pattern
	static CvConnectionService& GetInstance();

	// Initialize the connection service
	bool Setup();

	// Shutdown the connection service and cleanup resources
	void Shutdown();

	// Log messages at different levels
	enum LogLevel
	{
		LOG_DEBUG = 0,
		LOG_INFO = 1,
		LOG_WARNING = 2,
		LOG_ERROR = 3
	};

	// Log a message to the connection log file
	void Log(LogLevel level, const char* message);

private:
	// Private constructor for singleton
	CvConnectionService();
	~CvConnectionService();

	// Prevent copying
	CvConnectionService(const CvConnectionService&);
	CvConnectionService& operator=(const CvConnectionService&);

	// Get the log file for writing
	FILogFile* GetLogFile();

	// Named Pipe server thread function
	static DWORD WINAPI NamedPipeServerThread(LPVOID lpParam);
	
	// Handle a single client connection
	void HandleClientConnection(HANDLE hPipe);

	// Internal state
	bool m_bInitialized;
	
	// Named Pipe server state
	HANDLE m_hPipe;
	HANDLE m_hThread;
	DWORD m_dwThreadId;
	volatile bool m_bClientConnected;
	volatile bool m_bShutdownRequested;
};

#endif // CIV5_CONNECTION_SERVICE_H