// GameStatePipe.cpp - Threaded named pipe implementation for LLM integration

#include "CvGameCoreDLLPCH.h"
#include "GameStatePipe.h"
#include "CvGame.h"

#include <cstdarg>
#include <sstream>

#if defined(_WIN32)

// Shell API for log path
#ifndef CSIDL_PERSONAL
#define CSIDL_PERSONAL 0x0005
#endif
#ifndef CSIDL_FLAG_CREATE
#define CSIDL_FLAG_CREATE 0x8000
#endif
#ifndef SHGFP_TYPE_CURRENT
#define SHGFP_TYPE_CURRENT 0
#endif
extern "C" __declspec(dllimport) HRESULT __stdcall SHGetFolderPathA(HWND hwnd, int csidl, HANDLE hToken, DWORD dwFlags, LPSTR pszPath);
extern "C" __declspec(dllimport) int __stdcall SHCreateDirectoryExA(HWND hwnd, LPCSTR pszPath, const SECURITY_ATTRIBUTES* psd);

//------------------------------------------------------------------------------
// Configuration
//------------------------------------------------------------------------------
namespace
{
	const char* PIPE_NAME = "\\\\.\\pipe\\civv_llm";
	const char* LOG_DIR = "\\My Games\\Sid Meier's Civilization 5\\Logs\\LLMCiv";
	const char* LOG_FILE = "\\game_state_pipe.log";
	const DWORD PIPE_BUFFER_SIZE = 4096;
	const DWORD CONNECT_RETRY_MS = 1000;  // Wait between connection attempts
	const DWORD READ_TIMEOUT_MS = 100;    // Short timeout for responsive shutdown
}

//------------------------------------------------------------------------------
// Logging
//------------------------------------------------------------------------------
namespace
{
	std::string BuildLogPath()
	{
		char docsPath[MAX_PATH] = {0};
		if (FAILED(SHGetFolderPathA(NULL, CSIDL_PERSONAL | CSIDL_FLAG_CREATE, NULL, SHGFP_TYPE_CURRENT, docsPath)))
		{
			return std::string();
		}

		std::string directory = std::string(docsPath) + LOG_DIR;
		SHCreateDirectoryExA(NULL, directory.c_str(), NULL);
		return directory + LOG_FILE;
	}

	void LogMessage(const char* format, ...)
	{
		static std::string s_logPath = BuildLogPath();
		if (s_logPath.empty())
		{
			return;
		}

		char messageBuffer[1024];
		va_list args;
		va_start(args, format);
		_vsnprintf_s(messageBuffer, _TRUNCATE, format, args);
		va_end(args);

		SYSTEMTIME st;
		GetLocalTime(&st);
		char timestamp[64];
		_snprintf_s(timestamp, _TRUNCATE, "%04u-%02u-%02u %02u:%02u:%02u.%03u ",
		            st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

		std::string finalMessage = std::string(timestamp) + messageBuffer + "\r\n";

		HANDLE hFile = CreateFileA(s_logPath.c_str(), FILE_APPEND_DATA, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			DWORD written = 0;
			WriteFile(hFile, finalMessage.c_str(), static_cast<DWORD>(finalMessage.size()), &written, NULL);
			CloseHandle(hFile);
		}
	}
}

//------------------------------------------------------------------------------
// PipeJson namespace implementation
//------------------------------------------------------------------------------
namespace PipeJson
{
	std::string Escape(const std::string& value)
	{
		std::string escaped;
		escaped.reserve(value.size());
		for (size_t i = 0; i < value.size(); ++i)
		{
			char ch = value[i];
			switch (ch)
			{
			case '\\': escaped.append("\\\\"); break;
			case '\"': escaped.append("\\\""); break;
			case '\n': escaped.append("\\n"); break;
			case '\r': escaped.append("\\r"); break;
			case '\t': escaped.append("\\t"); break;
			default: escaped.push_back(ch); break;
			}
		}
		return escaped;
	}

	std::string GetString(const std::string& json, const char* key)
	{
		// Simple JSON string extraction - looks for "key":"value" or "key": "value"
		std::string searchKey = std::string("\"") + key + "\"";
		size_t keyPos = json.find(searchKey);
		if (keyPos == std::string::npos) return "";

		size_t colonPos = json.find(':', keyPos + searchKey.size());
		if (colonPos == std::string::npos) return "";

		size_t valueStart = json.find('\"', colonPos);
		if (valueStart == std::string::npos) return "";
		valueStart++;

		size_t valueEnd = valueStart;
		while (valueEnd < json.size())
		{
			if (json[valueEnd] == '\"' && (valueEnd == valueStart || json[valueEnd - 1] != '\\'))
			{
				break;
			}
			valueEnd++;
		}

		return json.substr(valueStart, valueEnd - valueStart);
	}

	bool HasKey(const std::string& json, const char* key)
	{
		std::string searchKey = std::string("\"") + key + "\"";
		return json.find(searchKey) != std::string::npos;
	}
}

//------------------------------------------------------------------------------
// CommandQueue implementation
//------------------------------------------------------------------------------
CommandQueue::CommandQueue()
{
	InitializeCriticalSection(&m_cs);
	m_head = NULL;
	m_tail = NULL;
	m_count = 0;
}

CommandQueue::~CommandQueue()
{
	Clear();
	DeleteCriticalSection(&m_cs);
}

void CommandQueue::Push(const std::string& command)
{
	Node* node = new Node();
	node->command = command;
	node->next = NULL;

	EnterCriticalSection(&m_cs);

	if (m_tail)
	{
		m_tail->next = node;
		m_tail = node;
	}
	else
	{
		m_head = m_tail = node;
	}
	m_count++;

	LeaveCriticalSection(&m_cs);
}

bool CommandQueue::TryPop(std::string& outCommand)
{
	EnterCriticalSection(&m_cs);

	if (!m_head)
	{
		LeaveCriticalSection(&m_cs);
		return false;
	}

	Node* node = m_head;
	outCommand = node->command;
	m_head = node->next;
	if (!m_head)
	{
		m_tail = NULL;
	}
	m_count--;

	LeaveCriticalSection(&m_cs);

	delete node;
	return true;
}

bool CommandQueue::HasCommands() const
{
	// Simple read, no lock needed for volatile bool check
	return m_head != NULL;
}

void CommandQueue::Clear()
{
	EnterCriticalSection(&m_cs);

	while (m_head)
	{
		Node* next = m_head->next;
		delete m_head;
		m_head = next;
	}
	m_tail = NULL;
	m_count = 0;

	LeaveCriticalSection(&m_cs);
}

//------------------------------------------------------------------------------
// GameStatePipe implementation
//------------------------------------------------------------------------------
GameStatePipe::GameStatePipe()
	: m_hPipe(INVALID_HANDLE_VALUE)
	, m_hThread(NULL)
	, m_hStopEvent(NULL)
	, m_bRunning(false)
	, m_bConnected(false)
{
	InitializeCriticalSection(&m_writeLock);
}

GameStatePipe::~GameStatePipe()
{
	Shutdown();
	DeleteCriticalSection(&m_writeLock);
}

void GameStatePipe::Initialize()
{
	if (m_bRunning)
	{
		LogMessage("GameStatePipe: Already running, skipping Initialize");
		return;
	}

	LogMessage("GameStatePipe: Initializing...");

	// Create stop event
	m_hStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (!m_hStopEvent)
	{
		LogMessage("GameStatePipe: Failed to create stop event, error=%lu", GetLastError());
		return;
	}

	m_bRunning = true;

	// Create pipe thread
	m_hThread = CreateThread(NULL, 0, PipeThreadProc, this, 0, NULL);
	if (!m_hThread)
	{
		LogMessage("GameStatePipe: Failed to create pipe thread, error=%lu", GetLastError());
		CloseHandle(m_hStopEvent);
		m_hStopEvent = NULL;
		m_bRunning = false;
		return;
	}

	LogMessage("GameStatePipe: Initialized successfully, thread started");
}

void GameStatePipe::Shutdown()
{
	if (!m_bRunning)
	{
		return;
	}

	LogMessage("GameStatePipe: Shutting down...");

	m_bRunning = false;

	// Signal thread to stop
	if (m_hStopEvent)
	{
		SetEvent(m_hStopEvent);
	}

	// Wait for thread to exit
	if (m_hThread)
	{
		WaitForSingleObject(m_hThread, 5000);  // Wait up to 5 seconds
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}

	// Clean up stop event
	if (m_hStopEvent)
	{
		CloseHandle(m_hStopEvent);
		m_hStopEvent = NULL;
	}

	// Disconnect pipe
	DisconnectPipe();

	// Clear any pending commands
	m_commandQueue.Clear();

	LogMessage("GameStatePipe: Shutdown complete");
}

bool GameStatePipe::IsConnected() const
{
	return m_bConnected;
}

bool GameStatePipe::IsRunning() const
{
	return m_bRunning;
}

//------------------------------------------------------------------------------
// Thread procedure
//------------------------------------------------------------------------------
DWORD WINAPI GameStatePipe::PipeThreadProc(LPVOID lpParam)
{
	GameStatePipe* pPipe = static_cast<GameStatePipe*>(lpParam);
	pPipe->PipeThreadMain();
	return 0;
}

void GameStatePipe::PipeThreadMain()
{
	LogMessage("GameStatePipe: Pipe thread started");

	while (m_bRunning)
	{
		// Try to connect if not connected
		if (!m_bConnected)
		{
			if (ConnectPipe())
			{
				LogMessage("GameStatePipe: Connected to pipe server");
			}
			else
			{
				// Wait before retry, but check for shutdown
				if (WaitForSingleObject(m_hStopEvent, CONNECT_RETRY_MS) == WAIT_OBJECT_0)
				{
					break;  // Shutdown requested
				}
				continue;
			}
		}

		// Read from pipe
		if (!ReadFromPipe())
		{
			// Read failed, disconnect and retry
			DisconnectPipe();
		}
	}

	LogMessage("GameStatePipe: Pipe thread exiting");
}

bool GameStatePipe::ConnectPipe()
{
	// Try to connect to the pipe server (orchestrator)
	HANDLE hPipe = CreateFileA(
		PIPE_NAME,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	if (hPipe == INVALID_HANDLE_VALUE)
	{
		DWORD error = GetLastError();
		// Only log occasionally to avoid spam
		static DWORD s_lastLogTime = 0;
		DWORD now = GetTickCount();
		if (now - s_lastLogTime > 10000)  // Log every 10 seconds
		{
			LogMessage("GameStatePipe: Pipe not available (error=%lu), waiting...", error);
			s_lastLogTime = now;
		}
		return false;
	}

	// Set pipe to message mode
	DWORD dwMode = PIPE_READMODE_BYTE;
	SetNamedPipeHandleState(hPipe, &dwMode, NULL, NULL);

	m_hPipe = hPipe;
	m_bConnected = true;
	return true;
}

void GameStatePipe::DisconnectPipe()
{
	if (m_hPipe != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hPipe);
		m_hPipe = INVALID_HANDLE_VALUE;
		m_bConnected = false;
		LogMessage("GameStatePipe: Disconnected from pipe");
	}
	m_readBuffer.clear();
}

bool GameStatePipe::ReadFromPipe()
{
	char buffer[PIPE_BUFFER_SIZE];
	DWORD bytesRead = 0;
	DWORD bytesAvailable = 0;

	// Check if data is available (non-blocking peek)
	if (!PeekNamedPipe(m_hPipe, NULL, 0, NULL, &bytesAvailable, NULL))
	{
		DWORD error = GetLastError();
		if (error == ERROR_BROKEN_PIPE || error == ERROR_PIPE_NOT_CONNECTED)
		{
			LogMessage("GameStatePipe: Pipe disconnected during peek");
			return false;
		}
		LogMessage("GameStatePipe: PeekNamedPipe failed, error=%lu", error);
		return false;
	}

	if (bytesAvailable == 0)
	{
		// No data available, wait a bit but check for shutdown
		if (WaitForSingleObject(m_hStopEvent, READ_TIMEOUT_MS) == WAIT_OBJECT_0)
		{
			return false;  // Shutdown requested
		}
		return true;  // No data but still connected
	}

	// Read available data
	if (!ReadFile(m_hPipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL))
	{
		DWORD error = GetLastError();
		if (error == ERROR_BROKEN_PIPE || error == ERROR_PIPE_NOT_CONNECTED)
		{
			LogMessage("GameStatePipe: Pipe disconnected during read");
			return false;
		}
		LogMessage("GameStatePipe: ReadFile failed, error=%lu", error);
		return false;
	}

	if (bytesRead == 0)
	{
		return true;  // No data read but still connected
	}

	// Append to buffer and process complete lines
	buffer[bytesRead] = '\0';
	m_readBuffer.append(buffer, bytesRead);

	// Process complete lines (newline-delimited JSON)
	size_t newlinePos;
	while ((newlinePos = m_readBuffer.find('\n')) != std::string::npos)
	{
		std::string line = m_readBuffer.substr(0, newlinePos);
		m_readBuffer.erase(0, newlinePos + 1);

		// Trim trailing \r if present
		if (!line.empty() && line[line.size() - 1] == '\r')
		{
			line.erase(line.size() - 1);
		}

		if (!line.empty())
		{
			LogMessage("GameStatePipe: Received command: %s", line.c_str());
			m_commandQueue.Push(line);
		}
	}

	return true;
}

//------------------------------------------------------------------------------
// Main thread operations
//------------------------------------------------------------------------------
void GameStatePipe::ProcessCommands(CvGame& game)
{
	// Process all queued commands
	std::string command;
	int processed = 0;
	const int MAX_COMMANDS_PER_FRAME = 10;  // Prevent blocking too long

	while (processed < MAX_COMMANDS_PER_FRAME && m_commandQueue.TryPop(command))
	{
		LogMessage("GameStatePipe: Processing command: %s", command.c_str());

		// Delegate to CvGame for actual command handling
		game.HandlePipeCommand(command);

		processed++;
	}
}

bool GameStatePipe::SendMessage(const std::string& message)
{
	if (!m_bConnected || m_hPipe == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	// Thread-safe write
	EnterCriticalSection(&m_writeLock);

	std::string payload = message;
	if (payload.empty() || payload[payload.size() - 1] != '\n')
	{
		payload.push_back('\n');
	}

	DWORD bytesWritten = 0;
	BOOL result = WriteFile(m_hPipe, payload.c_str(), static_cast<DWORD>(payload.size()), &bytesWritten, NULL);

	LeaveCriticalSection(&m_writeLock);

	if (!result || bytesWritten != payload.size())
	{
		LogMessage("GameStatePipe: Write failed, error=%lu", GetLastError());
		return false;
	}

	return true;
}

void GameStatePipe::Log(const char* format, ...)
{
	char buffer[1024];
	va_list args;
	va_start(args, format);
	_vsnprintf_s(buffer, _TRUNCATE, format, args);
	va_end(args);
	LogMessage("GameStatePipe: %s", buffer);
}

#else
// Non-Windows stubs

namespace PipeJson
{
	std::string Escape(const std::string& value) { return value; }
	std::string GetString(const std::string&, const char*) { return ""; }
	bool HasKey(const std::string&, const char*) { return false; }
}

CommandQueue::CommandQueue() {}
CommandQueue::~CommandQueue() {}
void CommandQueue::Push(const std::string&) {}
bool CommandQueue::TryPop(std::string&) { return false; }
bool CommandQueue::HasCommands() const { return false; }
void CommandQueue::Clear() {}

GameStatePipe::GameStatePipe() {}
GameStatePipe::~GameStatePipe() {}
void GameStatePipe::Initialize() {}
void GameStatePipe::Shutdown() {}
void GameStatePipe::ProcessCommands(CvGame&) {}
bool GameStatePipe::SendMessage(const std::string&) { return false; }
void GameStatePipe::Log(const char*, ...) {}
bool GameStatePipe::IsConnected() const { return false; }
bool GameStatePipe::IsRunning() const { return false; }

#endif
