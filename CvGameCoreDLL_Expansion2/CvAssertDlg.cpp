#include "CvGameCoreDLLPCH.h"
#include "CvAssert.h"
#include <time.h>

#ifdef VPDEBUG
#ifdef WIN32

// MessageBox constants
#define MB_OK                0x00000000L
#define MB_OKCANCEL         0x00000001L
#define MB_ABORTRETRYIGNORE 0x00000002L
#define MB_YESNOCANCEL      0x00000003L
#define MB_ICONERROR        0x00000010L
#define MB_TASKMODAL        0x00002000L

// MessageBox return values
#define IDOK     1
#define IDCANCEL 2
#define IDABORT  3
#define IDRETRY  4
#define IDIGNORE 5
#define IDYES    6
#define IDNO     7

// Windows functions
extern "C" {
    __declspec(dllimport) int __stdcall MessageBoxA(void* hWnd, const char* lpText, const char* lpCaption, unsigned int uType);
    __declspec(dllimport) unsigned long __stdcall GetTickCount(void);
}

// Assert tracking
struct AssertInfo {
    unsigned int count;
    unsigned int firstTime;
    unsigned int lastTime;
    unsigned int totalMemUsage;
    bool isPermanentlyIgnored;

    AssertInfo() : count(0), firstTime(0), lastTime(0), totalMemUsage(0), isPermanentlyIgnored(false) {}
};

static struct AssertTracker {
    std::map<std::string, AssertInfo> asserts;
    unsigned int sessionStart;
    unsigned int totalAsserts;
    FILE* logFile;

    AssertTracker() {
        sessionStart = GetTickCount();
        totalAsserts = 0;
        logFile = NULL;
        fopen_s(&logFile, "CvAssert.log", "a");
    }

    ~AssertTracker() {
        if (logFile) {
            fclose(logFile);
        }
    }

    void LogAssert(const char* expr, const char* file, unsigned int line, const char* msg) {
        if (!logFile) return;

        time_t rawtime;
        time(&rawtime);
        char timeBuffer[26];
        ctime_s(timeBuffer, sizeof(timeBuffer), &rawtime);
        timeBuffer[24] = '\0'; // Remove newline

        fprintf(logFile, "\n[%s] Assert in %s:%u\n", timeBuffer, file, line);
        fprintf(logFile, "Expression: %s\n", expr);
        if (msg && *msg) {
            fprintf(logFile, "Message: %s\n", msg);
        }
        fflush(logFile);
    }

    std::string GetAssertKey(const char* file, unsigned int line, const char* expr) {
        char key[512];
        _snprintf_s(key, _countof(key), _TRUNCATE, "%s:%u:%s", file, line, expr);
        return std::string(key);
    }

    void UpdateStats(const std::string& key) {
        AssertInfo& info = asserts[key];
        unsigned int currentTime = GetTickCount();

        if (info.count == 0) {
            info.firstTime = currentTime;
        }

        info.count++;
        info.lastTime = currentTime;
        totalAsserts++;
    }

} g_AssertTracker;

bool CvAssertDlg(const char* expr, const char* szFile, unsigned int uiLine, bool& bIgnoreAlways, const char* msg)
{
    if (!expr) return false;

    // Get unique key for this assert
    std::string assertKey = g_AssertTracker.GetAssertKey(szFile, uiLine, expr);

    // Check if permanently ignored
    if (g_AssertTracker.asserts[assertKey].isPermanentlyIgnored) {
        return false;
    }

    // Check if ignored for this session
    if (bIgnoreAlways) {
        return false;
    }

    // Log the assert
    g_AssertTracker.LogAssert(expr, szFile, uiLine, msg);

    // Update statistics
    g_AssertTracker.UpdateStats(assertKey);

    // Get assert info
    const AssertInfo& info = g_AssertTracker.asserts[assertKey];

    // Calculate timing information
    unsigned int timeSinceFirst = GetTickCount() - info.firstTime;
    unsigned int timeSinceLast = GetTickCount() - info.lastTime;
    unsigned int sessionTime = GetTickCount() - g_AssertTracker.sessionStart;

    // Format the message
    char szBuffer[4096];
    _snprintf_s(szBuffer, _countof(szBuffer), _TRUNCATE,
        "Assert Failed!\n"
        "==================\n"
        "Expression: %s\n"
        "File: %s\n"
        "Line: %u\n"
        "%s%s%s\n"
        "\nStatistics:\n"
        "==================\n"
        "This assert has fired %u time(s)\n"
        "First occurrence: %.2f seconds ago\n"
        "Last occurrence: %.2f seconds ago\n"
        "Total asserts this session: %u\n"
        "Session duration: %.2f seconds\n"
        "\nOptions:\n"
        "==================\n"
        "Yes - Break into debugger\n"
        "No - Continue execution\n"
        "Cancel - Ignore this assert",
        expr, szFile, uiLine,
        msg ? "\nMessage: " : "", msg ? msg : "", msg ? "\n" : "",
        info.count,
        timeSinceFirst / 1000.0f,
        timeSinceLast / 1000.0f,
        g_AssertTracker.totalAsserts,
        sessionTime / 1000.0f);

    // Show dialog
    int nResult = MessageBoxA(NULL, szBuffer, "Assertion Failed",
        MB_YESNOCANCEL | MB_ICONERROR | MB_TASKMODAL);

    // Handle result
    switch (nResult)
    {
    case IDYES:      // Break into debugger
        return true;

    case IDNO:       // Continue execution
        return false;

    default:         // Ignore always
        g_AssertTracker.asserts[assertKey].isPermanentlyIgnored = true;
        bIgnoreAlways = true;
        return false;
    }
}

#endif // WIN32
#endif // VPDEBUG