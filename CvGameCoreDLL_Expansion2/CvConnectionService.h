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
#include <map>
#include "../../ThirdPartyLibs/ArduinoJSON.hpp"

// Use ArduinoJson
using namespace ArduinoJson;

// Forward declarations
class FILogFile;
struct lua_State;

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
	
	// Send a message to the Bridge Service (called from game code)
	void SendMessage(const DynamicJsonDocument& message);

	// Register a Lua function for external calling
	void RegisterLuaFunction(const char* name, lua_State* L, int stackIndex);
	
	// Unregister a specific function
	void UnregisterLuaFunction(const char* name);
	
	// Clear all registered functions (called on shutdown)
	void ClearLuaFunctions();

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
	void QueueIncomingMessage(const std::string& messageJson);
	void QueueOutgoingMessage(const std::string& messageJson);
	bool DequeueOutgoingMessage(std::string& messageJson);
	
	// Message routing and handling
	void RouteMessage(const std::string& messageJson);
	
	// Lua execution handlers
	void HandleLuaExecute(const char* script, const char* id);
	void ProcessLuaResult(lua_State* L, int executionResult, const char* id);
	void ConvertLuaToJsonValue(lua_State* L, int index, JsonVariant dest);
	
	// Send registration notification to Bridge
	void NotifyBridgeOfRegistration(const char* name, const char* description);
	
	// Structure to store registered Lua functions
	struct LuaFunctionInfo {
		lua_State* pLuaState;      // The Lua state that registered this function
		int iRegistryRef;          // Reference in the Lua registry (LUA_REGISTRYINDEX)
		std::string strDescription; // Optional description for documentation
	};
	
	// Internal state
	bool m_bInitialized;
	lua_State* m_pLuaState;
	
	// Named Pipe server state
	HANDLE m_hPipe;
	HANDLE m_hThread;
	DWORD m_dwThreadId;
	volatile bool m_bClientConnected;
	volatile bool m_bShutdownRequested;
	
	// Thread-safe message queues (store serialized JSON strings)
	std::queue<std::string> m_incomingQueue;  // Bridge -> Game (stores stripped JSON)
	std::queue<std::string> m_outgoingQueue;  // Game -> Bridge (stores wrapped JSON)
	
	// Critical sections for thread safety
	CRITICAL_SECTION m_csIncoming;
	CRITICAL_SECTION m_csOutgoing;
	CRITICAL_SECTION m_csFunctions;
	
	// Map of function name to function info
	std::map<std::string, LuaFunctionInfo> m_registeredFunctions;
};

#endif // CIV5_CONNECTION_SERVICE_H