@echo off
setlocal enabledelayedexpansion

echo Building Vox Deorum DLL...
echo.

REM Call the Python build script with all arguments passed to this batch file
python build_vp_clang_sdk.py %*

REM Check if the build succeeded
if %ERRORLEVEL% neq 0 (
    echo.
    echo Build failed with error code %ERRORLEVEL%
    exit /b %ERRORLEVEL%
)

echo.
echo Build succeeded! Copying DLL to Civ5 MODS folder...

REM Get the actual Documents folder path using PowerShell
for /f "usebackq tokens=*" %%i in (`powershell -Command "[Environment]::GetFolderPath('MyDocuments')"`) do set "DOCUMENTS=%%i"

REM Construct the destination path
set "DEST_DIR=%DOCUMENTS%\My Games\Sid Meier's Civilization 5\MODS\(1) Community Patch"
set "SOURCE_DLL=clang-output\Debug\CvGameCore_Expansion2.dll"

REM Check if source DLL exists
if not exist "%SOURCE_DLL%" (
    echo Error: Built DLL not found at %SOURCE_DLL%
    exit /b 1
)

REM Create destination directory if it doesn't exist
if not exist "%DEST_DIR%" (
    echo Creating directory: %DEST_DIR%
    mkdir "%DEST_DIR%"
)

REM Copy the DLL, overwriting if it exists
echo Copying %SOURCE_DLL% to "%DEST_DIR%\"
copy /Y "%SOURCE_DLL%" "%DEST_DIR%\" >nul

if %ERRORLEVEL% equ 0 (
    echo.
    echo DLL successfully copied to Civ5 MODS folder!
    echo Location: %DEST_DIR%\CvGameCore_Expansion2.dll
) else (
    echo.
    echo Error: Failed to copy DLL to destination
    exit /b 1
)

echo.
echo Build and deployment complete!