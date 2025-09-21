@echo off
setlocal enabledelayedexpansion

REM Parse command line arguments
set "BUILD_TYPE=debug"
set "BUILD_BOTH=0"

:parse_args
if "%~1"=="" goto end_parse
if /i "%~1"=="--release" (
    set "BUILD_TYPE=release"
    shift
    goto parse_args
)
if /i "%~1"=="--both" (
    set "BUILD_BOTH=1"
    shift
    goto parse_args
)
shift
goto parse_args
:end_parse

REM Function to build and copy a specific configuration
goto :start_build

:build_and_copy
    set "CONFIG=%~1"
    set "CONFIG_DISPLAY=%~2"

    echo.
    echo ========================================
    echo Building Vox Deorum DLL (%CONFIG_DISPLAY%)...
    echo ========================================
    echo.

    REM Call the Python build script with configuration
    if /i "%CONFIG%"=="release" (
        python build_vp_clang_sdk.py --config release
    ) else (
        python build_vp_clang_sdk.py
    )

    REM Check if the build succeeded
    if !ERRORLEVEL! neq 0 (
        echo.
        echo ========================================
        echo BUILD FAILED for %CONFIG_DISPLAY% with error code !ERRORLEVEL!
        echo ========================================
        echo.
        echo Check the build log at: clang-output\%CONFIG_DISPLAY%\build.log
        echo.
        echo Common compilation errors to look for:
        echo - Syntax errors in C++ code
        echo - Missing include files
        echo - Access to private/protected members
        echo - Type mismatches
        echo.
        echo Showing compilation errors from build log:
        echo ----------------------------------------
        powershell -Command "if (Test-Path 'clang-output\%CONFIG_DISPLAY%\build.log') { Select-String -Path 'clang-output\%CONFIG_DISPLAY%\build.log' -Pattern 'error:' | Select-Object -Last 10 | ForEach-Object { $_.Line } }"
        echo.
        echo Showing last few lines of build log:
        echo ----------------------------------------
        powershell -Command "if (Test-Path 'clang-output\%CONFIG_DISPLAY%\build.log') { Get-Content 'clang-output\%CONFIG_DISPLAY%\build.log' | Select-Object -Last 15 }"
        echo ----------------------------------------
        echo.
        exit /b !ERRORLEVEL!
    )

    echo.
    echo Build succeeded! Copying %CONFIG_DISPLAY% DLL to Civ5 MODS folder...

    REM Get the actual Documents folder path using PowerShell
    for /f "usebackq tokens=*" %%i in (`powershell -Command "[Environment]::GetFolderPath('MyDocuments')"`) do set "DOCUMENTS=%%i"

    REM Construct the destination path
    set "DEST_DIR=!DOCUMENTS!\My Games\Sid Meier's Civilization 5\MODS\(1) Community Patch"
    set "SOURCE_DLL=clang-output\%CONFIG_DISPLAY%\CvGameCore_Expansion2.dll"
    set "SOURCE_PDB=clang-output\%CONFIG_DISPLAY%\CvGameCore_Expansion2.pdb"

    REM Check if source DLL exists
    if not exist "!SOURCE_DLL!" (
        echo Error: Built DLL not found at !SOURCE_DLL!
        exit /b 1
    )

    REM Create destination directory if it doesn't exist
    if not exist "!DEST_DIR!" (
        echo Creating directory: "!DEST_DIR!"
        mkdir "!DEST_DIR!" 2>nul
    )

    REM Copy the DLL, overwriting if it exists
    echo Copying "!SOURCE_DLL!" to "!DEST_DIR!"
    copy /Y "!SOURCE_DLL!" "!DEST_DIR!" >nul

    if !ERRORLEVEL! equ 0 (
        echo %CONFIG_DISPLAY% DLL successfully copied to Civ5 MODS folder!
        echo Location: "!DEST_DIR!\CvGameCore_Expansion2.dll"
    ) else (
        echo.
        echo Error: Failed to copy %CONFIG_DISPLAY% DLL to destination
        exit /b 1
    )

    REM Copy the PDB file if it exists (for debugging)
    if exist "!SOURCE_PDB!" (
        echo Copying %CONFIG_DISPLAY% PDB file for debugging...
        copy /Y "!SOURCE_PDB!" "!DEST_DIR!" >nul
        if !ERRORLEVEL! equ 0 (
            echo %CONFIG_DISPLAY% PDB file successfully copied!
        ) else (
            echo Warning: Failed to copy %CONFIG_DISPLAY% PDB file
        )
    ) else (
        echo Note: %CONFIG_DISPLAY% PDB file not found, skipping debug symbols copy
    )

    REM If building both, also copy to scripts directory
    if "%BUILD_BOTH%"=="1" (
        set "SCRIPTS_DIR=..\scripts\%CONFIG%"

        REM Create scripts directory if it doesn't exist
        if not exist "!SCRIPTS_DIR!" (
            echo Creating directory: "!SCRIPTS_DIR!"
            mkdir "!SCRIPTS_DIR!" 2>nul
        )

        REM Copy DLL to scripts directory
        echo Copying %CONFIG_DISPLAY% DLL to scripts directory...
        copy /Y "!SOURCE_DLL!" "!SCRIPTS_DIR!" >nul
        if !ERRORLEVEL! equ 0 (
            echo %CONFIG_DISPLAY% DLL copied to: "!SCRIPTS_DIR!\CvGameCore_Expansion2.dll"
        ) else (
            echo Warning: Failed to copy %CONFIG_DISPLAY% DLL to scripts directory
        )

        REM Copy PDB to scripts directory if it exists
        if exist "!SOURCE_PDB!" (
            copy /Y "!SOURCE_PDB!" "!SCRIPTS_DIR!" >nul
            if !ERRORLEVEL! equ 0 (
                echo %CONFIG_DISPLAY% PDB copied to: "!SCRIPTS_DIR!\CvGameCore_Expansion2.pdb"
            ) else (
                echo Warning: Failed to copy %CONFIG_DISPLAY% PDB to scripts directory
            )
        )
    )

    exit /b 0

:start_build

REM Build based on configuration
if "%BUILD_BOTH%"=="1" (
    echo Building both Debug and Release versions...

    REM Build Debug first
    call :build_and_copy debug Debug
    if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

    REM Build Release
    call :build_and_copy release Release
    if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

    echo.
    echo ========================================
    echo Both Debug and Release builds complete!
    echo ========================================
) else if /i "%BUILD_TYPE%"=="release" (
    call :build_and_copy release Release
    if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

    echo.
    echo ========================================
    echo Release build and deployment complete!
    echo ========================================
) else (
    call :build_and_copy debug Debug
    if !ERRORLEVEL! neq 0 exit /b !ERRORLEVEL!

    echo.
    echo ========================================
    echo Debug build and deployment complete!
    echo ========================================
)