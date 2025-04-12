@echo off
setlocal

REM Check if we're running in MSYS2 environment
if not "%MSYSTEM%"=="MINGW64" (
    echo Please run this script from MSYS2 MinGW 64-bit terminal
    echo You can find it in: Start Menu -> MSYS2 -> MSYS2 MinGW 64-bit
    exit /b 1
)

REM Check if required packages are installed
where pkg-config >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo pkg-config is not installed. Please run:
    echo pacman -S mingw-w64-x86_64-pkg-config
    exit /b 1
)

where cmake >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo CMake is not installed. Please run:
    echo pacman -S mingw-w64-x86_64-cmake
    exit /b 1
)

where gcc >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo MinGW is not installed. Please run:
    echo pacman -S mingw-w64-x86_64-gcc
    exit /b 1
)

REM Check for SDL2 packages
pkg-config --exists sdl2
if %ERRORLEVEL% neq 0 (
    echo SDL2 is not installed. Please run:
    echo pacman -S mingw-w64-x86_64-SDL2
    exit /b 1
)

pkg-config --exists SDL2_image
if %ERRORLEVEL% neq 0 (
    echo SDL2_image is not installed. Please run:
    echo pacman -S mingw-w64-x86_64-SDL2_image
    exit /b 1
)

pkg-config --exists SDL2_ttf
if %ERRORLEVEL% neq 0 (
    echo SDL2_ttf is not installed. Please run:
    echo pacman -S mingw-w64-x86_64-SDL2_ttf
    exit /b 1
)

REM Create build directory if it doesn't exist
if not exist build mkdir build
cd build

REM Run CMake
echo Configuring project with CMake...
cmake -G "MinGW Makefiles" ..

REM Build the project
echo Building project...
mingw32-make

if %ERRORLEVEL% neq 0 (
    echo Build failed
    exit /b 1
)

echo Build successful!
echo You can run the program with: .\habitica_clone.exe
cd .. 