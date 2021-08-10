cmake_minimum_required(VERSION 3.13)  # CMake version check
project(stase)               # Create project "simple_example"
set(CMAKE_CXX_STANDARD 14)            # Enable c++14 standard

# Add main.cpp file of project root directory as source file
set(SOURCE_FILES scratch.cpp src/search/search.cpp)

include_directories(include/stase)

file(GLOB SOURCES *.h *.cpp)

# Add executable target with source files listed in SOURCE_FILES variable
add_executable(stase ${SOURCE_FILES} ${SOURCES})