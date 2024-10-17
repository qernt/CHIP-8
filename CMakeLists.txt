cmake_minimum_required(VERSION 3.26)
project(CHIP_8)

set(CMAKE_CXX_STANDARD 23)

add_executable(CHIP_8 main.cpp
        chip8.cpp
        chip8.h)

# Find the SDL2 package
find_package(SDL2 REQUIRED)

# Include SDL2 header files
include_directories(${SDL2_INCLUDE_DIRS})

# Link SDL2 library
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
