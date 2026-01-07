# Minidump Guide

A minidump (crash dump) is a diagnostic file that captures the state of the game when an unhandled exception occurs. This guide explains how to generate, locate, and analyze minidumps to help diagnose crashes in the Community Patch DLL.

## Table of Contents
- [What are Minidumps?](#what-are-minidumps)
- [When are Minidumps Generated?](#when-are-minidumps-generated)
- [Locating Minidump Files](#locating-minidump-files)
- [Filename Format](#filename-format)
- [Analyzing Minidumps](#analyzing-minidumps)
  - [Using Visual Studio](#using-visual-studio)
  - [Using WinDbg](#using-windbg)
- [Submitting Minidumps](#submitting-minidumps)
- [Disabling Minidumps](#disabling-minidumps)
- [Additional Resources](#additional-resources)

## What are Minidumps?

Minidumps are diagnostic files containing a snapshot of the game's process memory at the moment a crash occurred. They include:
- Thread call stacks (showing the sequence of function calls)
- Function parameters and local variables
- CPU register states
- Exception information
- Version and build information

Minidumps can be shared with developers to help diagnose crashes that occurred on another machine, making them invaluable for bug reports.

## When are Minidumps Generated?

Minidumps are automatically generated when the game encounters an unhandled exception (crash) if the minidump feature is enabled in the DLL. The feature is enabled by default in both Debug and Release builds.

**Types of Minidumps:**
- **Debug builds:** Create comprehensive dumps including full memory, handles, thread info, and more (~100MB-1GB+)
- **Release builds:** Create minimal dumps with basic thread and stack information (~1-10MB)

## Locating Minidump Files

Minidump files are saved in the **same directory as the game executable** when a crash occurs.

**Typical locations:**
- **Steam:** `C:\Program Files (x86)\Steam\steamapps\common\Sid Meier's Civilization V\`
- **Non-Steam:** `C:\Program Files (x86)\2K Games\Sid Meier's Civilization V\`
- **Mod testing with ModBuddy:** The folder where you placed the compiled DLL

Look for files matching the pattern `CvMiniDump_*.dmp`.

## Filename Format

Minidump filenames follow this format:

```
CvMiniDump_YYYYMMDD_HHMMSS_<version>_<buildtype>.dmp
```

**Example:**
```
CvMiniDump_20250329_143025_4.16_a1b2c3d_Release.dmp
```

**Components:**
- `YYYYMMDD`: Date (Year, Month, Day)
- `HHMMSS`: Time (Hour, Minute, Second)
- `<version>`: DLL version (e.g., "4.16")
- `<commit>`: Git commit hash (e.g., "a1b2c3d")
- `<buildtype>`: Either "Debug" or "Release"

This naming helps identify when the crash occurred and which version of the DLL was running.

## Analyzing Minidumps

### Using Visual Studio

Visual Studio can open and analyze minidump files, but may have limitations with Release builds or when debug symbols (PDB files) are not available.

**Steps:**
1. Open Visual Studio (2008, 2013, 2019, or 2022)
2. File → Open → File and select the `.dmp` file
3. Click "Debug with Native Only" (or similar option)
4. Visual Studio will attempt to load symbols and show the call stack
5. Examine the **Call Stack** window to see where the crash occurred
6. Use the **Autos** or **Locals** windows to inspect variable values

**Limitations:**
- May require matching PDB files (symbol files) from the exact build
- Some Release builds may have limited information due to optimizations
- Visual Studio may refuse to load some dumps

### Using WinDbg

WinDbg is a more powerful alternative that can analyze dumps even when Visual Studio cannot. It's particularly useful for Release builds.

**Download:**
- Windows 10/11: [WinDbg Preview from Microsoft Store](https://aka.ms/windbg)
- Older Windows: [Windows SDK](https://developer.microsoft.com/windows/downloads/windows-sdk/)

**Basic Analysis Steps:**

1. **Open the minidump in WinDbg**
   - Launch WinDbg
   - Press `Ctrl+D` or File → Open Dump File
   - Select your `.dmp` file

2. **Set the symbol path** (if you have the matching PDB file)
   ```
   .sympath C:\path\to\VP\BuildOutput\Release
   ```
   Replace with the path where your PDB file is located.

3. **Reload symbols**
   ```
   !sym noisy
   .reload /i
   ```
   The `/i` flag forces loading even if there are minor mismatches.

4. **Display the exception context**
   ```
   .ecxr
   ```
   This loads the register state at the time of the exception.

5. **View the call stack**
   ```
   k
   ```
   This displays the function call stack showing where the crash occurred.

**Example Output:**
```
0:000> k
ChildEBP RetAddr
0019f530 64b28588 CvGameCore_Expansion2!CvPlayer::GetCity+0x42
0019f750 64b28876 CvGameCore_Expansion2!CvDiplomacyAI::DoUpdateOpinion+0x128
0019f77c 64b2a0f6 CvGameCore_Expansion2!CvGame::update+0xa6
```

The call stack shows:
- Function names
- Source file locations (if symbols are loaded)
- Line numbers (if available)
- Memory addresses

**Common Commands:**
- `k` - Display call stack
- `kb` - Display call stack with parameters
- `kn` - Display call stack with frame numbers
- `.exr -1` - Display exception record
- `r` - Show register values
- `lm` - List loaded modules
- `!analyze -v` - Automated crash analysis (verbose)

**Advanced Example Workflow:**

If the initial exception context shows internal Windows functions, you may need to find the original exception in the call stack:

```
0:000> .cxr
0:000> kb
```

Look for `ntdll!KiUserExceptionDispatcher` in the stack - the frame below it usually contains the context record address for the original exception.

```
10 0019ef18 0067ff41 0019ef30 0019ef80 0019ef30 ntdll!KiUserExceptionDispatcher+0xf
```

Use the context record address (third parameter, `0019ef80` in this example):

```
0:000> .cxr 0019ef80
```

Then examine the exception:

```
0:000> .exr 0019ef30
ExceptionAddress: 0067ff41 (CvGameCore_Expansion2!CvPlayer::GetCity+0x42)
   ExceptionCode: c0000005 (Access violation)
  ExceptionFlags: 00000000
NumberParameters: 2
   Parameter[0]: 00000000
   Parameter[1]: 00000000
Attempt to read from address 00000000
```

This shows an access violation trying to read from a null pointer at `CvPlayer::GetCity`.

## Submitting Minidumps

When reporting a crash:

1. **Locate the minidump file** in your game directory
2. **Zip the minidump file** - minidumps compress very well (use `.zip` format for GitHub compatibility)
3. **Note the exact version** you were running
4. **Describe what you were doing** when the crash occurred
5. **Attach to your bug report** on the forum or GitHub
6. **Zip your Logs folder** from `My Games\Sid Meier's Civilization V\Logs` and include it as well
   - See [DEVELOPMENT.md](../DEVELOPMENT.md#to-enable-logging-for-bug-reports) for how to enable logging

**Important:**
- Debug dumps can be very large (100MB-1GB+) - check if developers need Debug or Release dump
- Release dumps are usually sufficient for initial investigation
- Always zip your Logs folder separately and attach it along with the minidump

## Disabling Minidumps

If you need to disable minidump generation (not recommended for debugging):

1. Open `CvGameCoreDLL_Expansion2/CustomMods.h`
2. Find the line:
   ```cpp
   #define MOD_DEBUG_MINIDUMP
   ```
3. Comment it out:
   ```cpp
   // #define MOD_DEBUG_MINIDUMP
   ```
4. Rebuild the DLL

**Note:** If you disable minidumps, also set `GenerateDebugInfo=No` in the linker settings to reduce DLL size.

## Additional Resources

### Documentation
- [DEVELOPMENT.md](../DEVELOPMENT.md) - How to build and debug the DLL
- [GeneralsGameCode Crashdumps Wiki](https://github.com/TheSuperHackers/GeneralsGameCode/wiki/crashdumps) - Another excellent crash dump guide
- [Original Minidump Workflow Guide Issue](https://github.com/LoneGazebo/Community-Patch-DLL/issues/6459)

### External References
- [Effective Minidumps](http://www.debuginfo.com/articles/effminidumps.html) - Technical article on minidump types
- [WinDbg Cheat Sheet](https://github.com/repnz/windbg-cheat-sheet) - Quick reference for WinDbg commands

### Tools
- [WinDbg Preview](https://aka.ms/windbg) - Modern debugging tool from Microsoft
- [Visual Studio](https://visualstudio.microsoft.com/) - IDE with integrated debugging

---