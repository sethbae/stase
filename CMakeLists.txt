cmake_minimum_required(VERSION 3.13)  # CMake version check
project(stase)               # Create project "simple_example"
set(CMAKE_CXX_STANDARD 14)            # Enable c++14 standard

# Add main.cpp file of project root directory as source file
set(SOURCE_FILES
        src/board/bitmap.cpp
        src/board/board.cpp
        src/board/helper.cpp
        src/board/move.cpp
        src/board/square.cpp

        src/display/display.cpp

        src/game/cands.cpp
        src/game/eval.cpp
        src/game/gamestate.cpp
        src/game/heur.cpp
        src/game/piece.cpp

        src/puzzle/puzzle.cpp

        src/search/search.cpp
        )

set(SCRATCH_SOURCE_FILES
        scratch.cpp)

set(BENCH_SOURCE_FILES
        src/bench/bench.h
        src/bench/benchgame.cpp
        src/bench/benchboard.cpp
        src/bench/bench_main.cpp)

include_directories(include/stase)

# Add executable target with source files listed in SOURCE_FILES variable
add_executable(stase ${SOURCE_FILES} ${SCRATCH_SOURCE_FILES} ${SOURCES})

add_executable(bench ${SOURCE_FILES} ${BENCH_SOURCE_FILES} ${SOURCES})