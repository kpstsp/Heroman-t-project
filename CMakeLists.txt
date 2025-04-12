cmake_minimum_required(VERSION 3.10)
project(heroman_project C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Set MinGW specific paths
if(WIN32)
    set(CMAKE_C_COMPILER "gcc")
    set(CMAKE_CXX_COMPILER "g++")
    set(CMAKE_RC_COMPILER "windres")
endif()

# Set MSYS2 specific paths
if(MSYS)
    # Set MSYS2 specific include and library paths
    set(CMAKE_PREFIX_PATH "/mingw64")
    set(CMAKE_INCLUDE_PATH "/mingw64/include")
    set(CMAKE_LIBRARY_PATH "/mingw64/lib")
endif()

# Find required packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)
pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)
pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)
find_package(SQLite3 REQUIRED)

# Add source files
set(SOURCES
    src/main.c
    src/game.c
    src/database.c
    src/player.c
    src/tasks.c
    src/ui.c
    src/sprites.c
)

# Add header files
set(HEADERS
    include/game.h
    include/database.h
    include/player.h
    include/tasks.h
    include/ui.h
    include/sprites.h
)

# Create executable
add_executable(heroman_project ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(heroman_project PRIVATE 
    include
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_IMAGE_INCLUDE_DIRS}
    ${SDL2_TTF_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(heroman_project
    ${SDL2_LIBRARIES}
    ${SDL2_IMAGE_LIBRARIES}
    ${SDL2_TTF_LIBRARIES}
    ${SQLite3_LIBRARIES}
)

# Copy assets directory to build directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR}) 