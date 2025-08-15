/*	-------------------------------------------------------------------------------------------------------
	© 2024 Vox Deorum Development Team. Connection Service for Bridge Communication.
	------------------------------------------------------------------------------------------------------- */
#pragma once

#ifndef CIV5_CONNECTION_SERVICE_H
#define CIV5_CONNECTION_SERVICE_H

#include <windows.h>
#include <string>
#include <queue>
#include <vector>
#include "../../ThirdPartyLibs/ArduinoJSON.hpp"

// Use ArduinoJson
using namespace ArduinoJson;

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

	// Process queued messages from the main game thread
	void ProcessMessages();

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

	// Queue management methods
	void QueueIncomingMessage(const DynamicJsonDocument& messageData);
	void QueueOutgoingMessage(const DynamicJsonDocument& messageData);
	bool DequeueOutgoingMessage(DynamicJsonDocument& messageData);
	
	// Message routing and handling
	void RouteMessage(const DynamicJsonDocument& message);
	
	// Internal state
	bool m_bInitialized;
	
	// Named Pipe server state
	HANDLE m_hPipe;
	HANDLE m_hThread;
	DWORD m_dwThreadId;
	volatile bool m_bClientConnected;
	volatile bool m_bShutdownRequested;
	
	// Thread-safe message queues
	std::queue<DynamicJsonDocument> m_incomingQueue;  // Bridge -> Game
	std::queue<DynamicJsonDocument> m_outgoingQueue;  // Game -> Bridge
	
	// Critical sections for thread safety
	CRITICAL_SECTION m_csIncoming;
	CRITICAL_SECTION m_csOutgoing;
};

#endif // CIV5_CONNECTION_SERVICE_H