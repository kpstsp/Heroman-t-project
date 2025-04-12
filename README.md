# Habitica Desktop Clone

A desktop clone of Habitica built with C, SDL2, and SQLite. This application allows you to track your habits, daily tasks, and to-dos in a gamified way, all while working offline.

## Features

- Task management (Habits, Dailies, and To-Dos)
- Player progression system
- Offline functionality
- Data persistence using SQLite
- Modern UI using SDL2

## Requirements

- Windows 10
- MSYS2 with MinGW-w64
- CMake
- SDL2
- SDL2_image
- SDL2_ttf
- SQLite3

## Installation

1. Install MSYS2:
   - Download from https://www.msys2.org/
   - Run the installer and follow the instructions
   - Open "MSYS2 MinGW 64-bit" from the Start menu
   - Update the package database and core packages:
     ```bash
     pacman -Syu
     ```
   - Close and reopen the terminal, then run:
     ```bash
     pacman -Syu
     ```

2. Install required packages in MSYS2:
   ```bash
   pacman -S mingw-w64-x86_64-gcc \
             mingw-w64-x86_64-cmake \
             mingw-w64-x86_64-SDL2 \
             mingw-w64-x86_64-SDL2_image \
             mingw-w64-x86_64-SDL2_ttf \
             mingw-w64-x86_64-sqlite3 \
             mingw-w64-x86_64-make
   ```

3. Clone this repository:
   ```bash
   git clone https://github.com/yourusername/habitica-clone.git
   cd habitica-clone
   ```

4. Build the project:
   - Open "MSYS2 MinGW 64-bit" from the Start menu
   - Navigate to the project directory
   - Run the build script:
     ```bash
     ./build.bat
     ```

## Running the Application

After building, you can run the application from the build directory:
```bash
./habitica_clone.exe
```

## Project Structure

- `src/` - Source files
  - `main.c` - Entry point
  - `game.c` - Core game logic
  - `database.c` - Database operations
- `include/` - Header files
  - `game.h` - Game structures and declarations
  - `database.h` - Database function declarations
- `assets/` - Game assets (fonts, images, etc.)
- `build.bat` - Windows build script

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments

- Inspired by Habitica (https://habitica.com)
- Built with SDL2 (https://www.libsdl.org)
- Uses SQLite for data storage (https://www.sqlite.org) 