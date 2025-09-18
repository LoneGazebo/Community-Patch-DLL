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
	// Returns the length of the outgoing queue after adding the message
	int SendMessage(const DynamicJsonDocument& message);

	// Register a Lua function for external calling
	void RegisterLuaFunction(const char* name, lua_State* L, int stackIndex);
	
	// Unregister a specific function
	void UnregisterLuaFunction(const char* name);

	// Forward game events to the Bridge Service
	void ForwardGameEvent(const char* eventName, class ICvEngineScriptSystemArgs1* args);
	
	// Save state (can be called when game is being saved)
	void SerializeEventSequence();
	
	// Broadcast a custom event with JSON payload
	void BroadcastEvent(const char* eventName, const DynamicJsonDocument& payload);

	// External function registry management
	void RegisterExternalFunction(const char* name, bool bAsync);
	void UnregisterExternalFunction(const char* name);
	bool IsExternalFunctionRegistered(const char* name) const;
	
	// External function call result structure
	struct ExternalCallResult
	{
		bool bSuccess;           // Whether the call succeeded
		std::string strError;    // Error message if failed
		std::string strData;     // JSON response data as string
	};
	
	// Callback type for external function calls
	typedef void (*ExternalCallCallback)(const ExternalCallResult& result, void* userData);
	
	// Call an external function (function name and callback extracted from Lua state)
	// Returns the number of values pushed onto the Lua stack
	int CallExternalFunction(lua_State* L);
	
	// Convert JSON to Lua value (made public for use in Lua callbacks)
	void ConvertJsonToLuaValue(lua_State* L, const JsonVariant& value);

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
	// Returns the length of the outgoing queue after adding the message
	int QueueOutgoingMessage(const std::string& messageJson);
	bool DequeueOutgoingMessage(std::string& messageJson);
	
	// Message routing and handling
	void RouteMessage(const std::string& messageJson);
	
	// Lua execution handlers
	void HandleLuaExecute(const char* script, const char* id);
	void HandleLuaCall(const char* functionName, const JsonArray& args, const char* id);
	void ProcessLuaResult(lua_State* L, int executionResult, const char* id);
	void ConvertLuaToJsonValue(lua_State* L, int index, JsonVariant parent, const char* key);
	
	// Shared function to convert Lua values to fill a key in a JsonDocument
	void ConvertLuaValuesInDocument(lua_State* L, int firstIndex, int numValues, DynamicJsonDocument& parent, const char* key);
	
	// Structure to store registered Lua functions
	struct LuaFunctionInfo {
		lua_State* pLuaState;      // The Lua state that registered this function
		int iRegistryRef;          // Reference in the Lua registry (LUA_REGISTRYINDEX)
		std::string strDescription; // Optional description for documentation
	};
	
	// Structure to store external function information
	struct ExternalFunctionInfo {
		std::string strName;       // Function name
		bool bAsync;               // Whether the function is async
		bool bRegistered;          // Registration status
	};
	
	// Structure to store pending async external calls
	struct PendingExternalCall {
		std::string strCallId;              // Unique ID for this call
		std::string strFunctionName;        // Function being called
		ExternalCallCallback pCallback;     // Callback function
		void* pUserData;                    // User data for callback
		DWORD dwTimestamp;                   // When the call was made
	};
	
	// Internal state
	bool m_bInitialized;
	lua_State* m_pLuaState;
	ICvEngineScriptSystem1* m_pkScriptSystem;  // VOX: Cached script system instance
	
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
	CRITICAL_SECTION m_csExternalFunctions;
	
	// Map of function name to function info
	std::map<std::string, LuaFunctionInfo> m_registeredFunctions;
	
	// Map of external function name to function info
	std::map<std::string, ExternalFunctionInfo> m_externalFunctions;
	
	// Map of pending external calls (call ID to pending call info)
	std::map<std::string, PendingExternalCall> m_pendingExternalCalls;
	CRITICAL_SECTION m_csPendingCalls;
	
	// Map to store results for synchronous calls (call ID to result)
	std::map<std::string, ExternalCallResult> m_syncCallResults;
	
	// Counter for generating unique call IDs
	unsigned int m_uiNextCallId;
	
	// Event tracking for proper event IDs
	unsigned int m_uiCurrentTurn;
	unsigned int m_uiEventSequence;
	
	// Helper to generate unique call ID
	std::string GenerateCallId();
	
	// Helper to generate event ID based on turn and sequence
	unsigned long long GenerateEventId();
	
	// Serialization methods for event sequence
	void DeserializeEventSequence();

	// Track last Lua GC time (for periodic garbage collection)
	DWORD m_dwLastGCTime;
	
	// Dynamic memory buffers for main thread message processing
	// Separate buffers to prevent memory corruption during nested operations
	// Allocated during Setup() to avoid unnecessary memory usage when IPC is disabled
	DynamicJsonDocument* m_pMainThreadReadBuffer;   // For deserializing incoming messages
	DynamicJsonDocument* m_pMainThreadWriteBuffer;  // For serializing outgoing messages
	
	// Helper to handle external call response
	void HandleExternalCallResponse(const char* callId, bool bSuccess, const char* error, const char* data);
	
	// Helper functions for external call validation
	bool ValidateExternalCall(const char* functionName, ExternalCallResult& result);
	
	// Shared post-processing function for external call results
	void ProcessExternalCallResult(lua_State* L, const ExternalCallResult& result);
	
	// Static callback handler for external function calls
	static void HandleExternalCallCallback(const ExternalCallResult& result, void* userData);
};

#endif // CIV5_CONNECTION_SERVICE_H