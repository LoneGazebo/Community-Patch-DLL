#pragma once

// GameStatePipe - Threaded named pipe communication for LLM integration
//
// Architecture:
// - Background thread owns pipe connection and reads continuously
// - Commands are queued in a thread-safe queue
// - Main game thread processes queued commands during update()
// - Responses are sent directly on the main thread
//
// This design ensures commands are received immediately regardless of
// game state, solving the "only polls between turns" problem.

#include <string>

#if defined(_WIN32)
#include <windows.h>
#endif

class CvGame;

// Forward declarations for internal types
struct PipeCommand;

// Thread-safe command queue
class CommandQueue
{
public:
	CommandQueue();
	~CommandQueue();

	// Push a command (called from pipe thread)
	void Push(const std::string& command);

	// Pop a command if available (called from main thread)
	// Returns true if a command was available
	bool TryPop(std::string& outCommand);

	// Check if queue has commands without popping
	bool HasCommands() const;

	// Clear all commands
	void Clear();

private:
#if defined(_WIN32)
	CRITICAL_SECTION m_cs;

	// Simple linked list node for the queue
	struct Node
	{
		std::string command;
		Node* next;
	};

	Node* m_head;
	Node* m_tail;
	int m_count;
#endif
};

// Custom message for waking up the main thread
#define WM_PIPE_COMMAND_READY (WM_USER + 0x1337)

// Main pipe communication class
class GameStatePipe
{
public:
	GameStatePipe();
	~GameStatePipe();

	// Lifecycle - pass CvGame pointer for window proc callback
	void Initialize(CvGame* pGame);
	void Shutdown();

	// Called from main thread during game update to process queued commands
	void ProcessCommands(CvGame& game);

	// Send a message to the connected client (called from main thread)
	bool SendMessage(const std::string& message);

	// Logging
	void Log(const char* format, ...);

	// Status
	bool IsConnected() const;
	bool IsRunning() const;

private:
#if defined(_WIN32)
	// Thread management
	static DWORD WINAPI PipeThreadProc(LPVOID lpParam);
	void PipeThreadMain();

	// Window subclassing for main thread wakeup
	static LRESULT CALLBACK SubclassWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	bool SubclassGameWindow();
	void UnsubclassGameWindow();

	// Pipe operations (called from pipe thread)
	bool ConnectPipe();
	void DisconnectPipe();
	bool ReadFromPipe();

	// Notify main thread that commands are waiting
	void NotifyMainThread();

	// State
	HANDLE m_hPipe;
	HANDLE m_hThread;
	HANDLE m_hStopEvent;
	volatile bool m_bRunning;
	volatile bool m_bConnected;

	// Window subclassing state
	HWND m_hGameWnd;
	LONG_PTR m_pfnOriginalWndProc;  // Stored as LONG_PTR, cast to WNDPROC when calling
	CvGame* m_pGame;  // For WndProc callback

	// Static instance pointer for WndProc access
	static GameStatePipe* s_pInstance;

	// Command queue
	CommandQueue m_commandQueue;

	// Partial message buffer (for handling messages split across reads)
	std::string m_readBuffer;

	// Write lock for thread-safe sending
	CRITICAL_SECTION m_writeLock;
#endif
};

// JSON utility functions
namespace PipeJson
{
	// Escape a string for JSON output
	std::string Escape(const std::string& value);

	// Simple JSON value extraction (returns empty string if not found)
	std::string GetString(const std::string& json, const char* key);

	// Check if JSON has a specific key
	bool HasKey(const std::string& json, const char* key);
}
