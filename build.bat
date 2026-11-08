@echo off
REM Build script for GD Mod Menu (Windows with Visual Studio / clang)

set PROJECT_NAME=gd-mod-menu
set BUILD_DIR=build

echo === Building %PROJECT_NAME% ===

REM Clean previous build
if exist %BUILD_DIR% rmdir /s /q %BUILD_DIR%
mkdir %BUILD_DIR%
cd %BUILD_DIR%

REM Configure with CMake for 32-bit Windows
cmake .. -G "Visual Studio 17 2022" -A Win32 -DCMAKE_BUILD_TYPE=Release

REM Build
cmake --build . --config Release

REM Copy DLL to output
if exist "Release\%PROJECT_NAME%.dll" (
    copy "Release\%PROJECT_NAME%.dll" "..\%PROJECT_NAME%.dll"
    echo Build successful! Output: %PROJECT_NAME%.dll
) else if exist "%PROJECT_NAME%.dll" (
    copy "%PROJECT_NAME%.dll" "..\%PROJECT_NAME%.dll"
    echo Build successful! Output: %PROJECT_NAME%.dll
) else (
    echo Build failed - DLL not found
    exit /b 1
)

cd ..