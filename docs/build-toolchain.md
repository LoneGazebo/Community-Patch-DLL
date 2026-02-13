# Build Toolchain Guide

This guide explains the Visual Studio toolchain requirements for building the Community Patch DLL, which uses the v90 platform toolset (Visual C++ 2008).

## Table of Contents
- [Overview](#overview)
- [Why Both VS2008 SP1 and VS2010 SP1 Are Required](#why-both-vs2008-sp1-and-vs2010-sp1-are-required)
- [Download Links](#download-links)
- [Installation Order](#installation-order)
- [Troubleshooting](#troubleshooting)
- [Alternative: Clang Build](#alternative-clang-build)
- [Additional Resources](#additional-resources)

## Overview

| Component | Required | Purpose |
|-----------|----------|---------|
| Visual Studio 2019 or 2022 | Yes | Host IDE for development |
| Visual Studio 2008 SP1 | Yes | Provides VC9 compiler and libraries |
| Visual Studio 2010 SP1 | Yes | Provides MSBuild toolset integration |

This project targets **Win32 (x86) only** and uses **C++03/TR1** features.

## Why Both VS2008 SP1 and VS2010 SP1 Are Required

When building a project with `PlatformToolset = v90` in modern Visual Studio (2019/2022), you need **two separate components** that serve different purposes.

### Visual Studio 2008 SP1 — The Compiler

VS2008 SP1 provides the actual build tools:
- **VC9 compiler** (`cl.exe`) and linker
- **C Runtime (CRT) libraries**
- **Headers and libraries** for Win32 development

**Why SP1 specifically?**
- SP1 includes important compiler and codegen bug fixes
- SP1 adds STL headers like `<array>` that the project depends on
- Binary compatibility requires the SP1 version of the libraries

**Without VS2008 SP1, you cannot compile with the v90 toolset.**

### Visual Studio 2010 SP1 — The MSBuild Integration

VS2010 SP1 provides the build system integration that modern Visual Studio relies on:
- **MSBuild v4.0 C++ infrastructure**
- **Platform toolset definitions** (`PlatformToolsets\v90\`)
- **Props and targets files** under `MSBuild\Microsoft.Cpp\v4.0\`
- **Registry entries** that VS2019/2022 use for toolset discovery

**Why is this needed?**

VS2008 predates the MSBuild-based `.vcxproj` format. When Microsoft moved C++ projects to MSBuild in VS2010, they included backward-compatible toolset definitions for v90. Modern Visual Studio relies on these MSBuild integration files to locate and invoke the VC9 compiler.

**Without VS2010 SP1**, you'll see errors like:
```
MSB8020: The build tools for Visual Studio 2008 (Platform Toolset = 'v90') cannot be found.
```
...even when VS2008 is correctly installed. This is because modern VS can find the compiler but lacks the MSBuild configuration files needed to invoke it.

### Summary

| Install | Provides | Required For |
|---------|----------|--------------|
| VS2008 SP1 | VC9 compiler, linker, CRT, headers | Actual compilation |
| VS2010 SP1 | MSBuild props/targets, toolset definitions | Modern VS to discover and use v90 |
| VS2019/2022 | Host IDE, modern MSBuild engine | Development environment |

## Download Links

### Visual Studio 2008 Express with SP1
**ISO image (~900MB)**

```
https://web.archive.org/web/20250618154620/https://download.microsoft.com/download/E/8/E/E8EEB394-7F42-4963-A2D8-29559B738298/VS2008ExpressWithSP1ENUX1504728.iso
```

### Visual Studio 2010 Express
**ISO image (~700MB)**
```
https://web.archive.org/web/20140424044344if_/http://download.microsoft.com/download/1/E/5/1E5F1C0A-0D5B-426A-A603-1798B951DDAE/VS2010Express1.iso
```

### Visual Studio 2010 SP1 Update
**Executable (~500MB)**
```
https://web.archive.org/web/20200812094150/https://download.microsoft.com/download/A/5/D/A5D85122-E8BB-4C29-8B87-1653058C0DA0/VS10SP1-KB2736182.exe
```

## Installation Order

Install in chronological order to ensure proper registry and file system setup:

1. **Visual Studio 2008 Express with SP1** (from ISO)
   - Mount or extract the ISO
   - Run the installer, select Visual C++ components
   
2. **Visual Studio 2010 Express** (from ISO)
   - Mount or extract the ISO
   - Run the installer, select Visual C++ components
   
3. **Visual Studio 2010 SP1** (executable)
   - Run the SP1 update installer
   
4. **Visual Studio 2019 or 2022** (your development IDE)
   - Install from Microsoft's website
   - Select "Desktop development with C++" workload

## Troubleshooting

### "Platform Toolset v90 cannot be found"
This typically means VS2010 SP1 is not installed. The MSBuild integration files are missing. Install VS2010 and apply SP1.

### STL headers like `<array>` not found
You have VS2008 installed but not SP1. The base VS2008 release lacks TR1 headers. Ensure you installed the "with SP1" version or apply the SP1 update.

### Build hangs at end of pass 1 (Release builds)
This is normal when Whole Program Optimization is enabled. The compiler is still working—it can take several minutes. You can disable WPO locally under Project > Properties > C/C++ > Optimization.

### Build stops responding at end of pass 2
Delete the hidden `.vs` folder and the `BuildOutput`/`BuildTemp` folders, then reopen the solution.

## Alternative: Clang Build

If you prefer not to use the MSVC toolchain in Visual Studio, there are Python-based clang build scripts available:

| Script | Requirements | Use Case |
|--------|--------------|----------|
| `build_vp_clang.py` | VS2008 installed (uses `VS90COMNTOOLS` env var) | Local development |
| `build_vp_clang_sdk.py` | Windows SDK 7.0 with VC9 components | CI builds, no VS install needed |

### How the SDK Script Works

The `build_vp_clang_sdk.py` script uses:
- **Windows SDK 7.0** for Windows headers/libs (`C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0\`)
- **VC9 headers/libs** from SDK 7.0's `vc_stdx86.msi` component, which installs to `C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\`

This is what CI uses—it downloads SDK 7.0 ISO and installs the necessary components without requiring VS2008 or VS2010.

### SDK Version Notes

- **SDK 7.0** - Standalone download, includes VC9 compiler components via `vc_stdx86.msi`
- **SDK 7.0A** - Bundled with VS2010 (the "A" suffix indicates VS2010 origin), same content at path `v7.0A\`

For local development, use `build_vp_clang.py` with VS2008 installed. For CI or environments without Visual Studio, use `build_vp_clang_sdk.py` with SDK 7.0.

See `DEVELOPMENT.md` for more details on the clang build option.

## Additional Resources

### Detailed Visual Guides
- [How to Compile the Vox Populi DLL](https://forums.civfanatics.com/threads/how-to-compile-the-vox-populi-dll.665916/) - Step-by-step tutorial with screenshots by ASCII Guy
- [How to Compile the DLL](https://forums.civfanatics.com/threads/how-to-compile-the-dll.608137/) - Original CivFanatics guide

### Technical Background
- [StackOverflow: Platform toolset v90 not found](https://stackoverflow.com/questions/76683920/platform-toolset-v90-not-found) - Why VS2010 provides MSBuild integration for v90
- [StackOverflow: Build with v90 without VS2008 using Windows SDK](https://stackoverflow.com/questions/24775363/how-to-build-with-v90-platform-toolset-in-vs2012-without-vs2008-using-windows-s) - How to use SDK 7.0 for CI builds without VS2008 installed

### Related Documentation
- [DEVELOPMENT.md](../DEVELOPMENT.md) - Main development guide
- [Minidump Guide](minidumps.md) - Crash dump analysis

---
