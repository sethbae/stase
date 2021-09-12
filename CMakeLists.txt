cmake_minimum_required(VERSION 3.13)
project(stase)
set(CMAKE_CXX_STANDARD 14)

set(OPT_FLAGS -Ofast)

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${OPT_FLAGS}")

set(SOURCE_FILES
        src/board/bitmap.cpp
        src/board/board.cpp
        src/board/helper.cpp
        src/board/move.cpp
        src/board/square.cpp

        src/display/display.cpp

        src/game/eval.cpp
        src/game/gamestate.cpp
        src/game/piece.cpp
        src/game/helper.h
        src/game/helper.cpp

        src/game/heur/heur.cpp
        src/game/heur/heur.h
        src/game/heur/piece_activity.cpp
        src/game/heur/control.cpp
        src/game/heur/pawns.cpp
        src/game/heur/development.cpp
        src/game/heur/king.cpp

        src/puzzle/puzzle.cpp

        src/search/search.cpp

        src/game/cands/cands.h
        src/game/cands/cands.cpp
        src/game/cands/weak.cpp
        src/game/cands/feature.cpp
        src/game/cands/defend.cpp
        src/game/cands/capture_piece.cpp)

set(STASE_SOURCE_FILES
        src/main.cpp)

set(SCRATCH_SOURCE_FILES
        scratch.cpp)

set(BENCH_SOURCE_FILES
        src/bench/bench.h
        src/bench/bench_game.cpp
        src/bench/bench_board.cpp
        src/bench/bench_main.cpp)

set(TEST_SOURCE_FILES
        src/test/test.h
        src/test/test_main.cpp
        src/test/board/test_board.cpp
        src/test/game/test_game.cpp
        src/test/test_helpers.cpp
        src/test/game/test_piece.cpp
        src/test/game/cands/test_capture_piece.cpp
        src/test/game/cands/test_weak_hook.cpp
        src/test/game/cands/test_defend.cpp)

include_directories(include/stase)

add_executable(stase ${SOURCE_FILES} ${STASE_SOURCE_FILES} ${SOURCES})

add_executable(scratch ${SOURCE_FILES} ${SCRATCH_SOURCE_FILES} ${SOURCES})

add_executable(bench ${SOURCE_FILES} ${BENCH_SOURCE_FILES} ${SOURCES})

add_executable(test ${SOURCE_FILES} ${TEST_SOURCE_FILES} ${SOURCES})