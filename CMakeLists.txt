cmake_minimum_required(VERSION 3.13)
project(stase)
set(CMAKE_CXX_STANDARD 20)

set(OPT_FLAGS -Ofast)

SET(CMAKE_CXX_FLAGS "-pthread ${CMAKE_CXX_FLAGS} ${OPT_FLAGS}")

set(SOURCE_FILES
        src/board/bitmap.cpp
        src/board/board.cpp
        src/board/helper.cpp
        src/board/move.cpp
        src/board/square.cpp
        src/board/piece.cpp

        src/display/display.cpp

        src/game/eval.cpp
        src/game/gamestate.cpp
        src/game/geometry.cpp
        src/game/piece.cpp
        src/game/helper.h
        src/game/helper.cpp
        src/game/weak.cpp
        src/game/king.cpp

        src/game/heur/heur.cpp
        src/game/heur/heur.h
        src/game/heur/piece_activity.cpp
        src/game/heur/control.cpp
        src/game/heur/pawns.cpp
        src/game/heur/development.cpp
        src/game/heur/king.cpp

        src/puzzle/puzzle.cpp
        src/puzzle/read_formats.cpp

        src/search/search.cpp
        src/search/thread.cpp
        src/search/metrics.h

        src/game/cands/cands.h
        src/game/cands/cands.cpp
        src/game/cands/unsafe_piece.cpp
        src/game/cands/defend.cpp
        src/game/cands/capture_piece.cpp
        src/game/cands/develop_piece.cpp
        src/game/cands/fork.cpp
        src/game/cands/trade_retreat_desperado.cpp
        src/game/cands/checks.cpp

        src/utils/utils.h
        src/utils/print.cpp)

set(STASE_SOURCE_FILES)

set(SCRATCH_SOURCE_FILES)

set(BENCH_SOURCE_FILES
        src/bench/bench.h
        src/bench/game/bench_game.cpp
        src/bench/board/bench_board.cpp
        src/bench/game/cands/bench_cands.cpp
        src/bench/game/heur/bench_heur.cpp)

set(TEST_SOURCE_FILES
        src/test/test.h
        src/test/stress_test_main.cpp
        src/test/board/test_board.cpp
        src/test/game/test_game.cpp
        src/test/test_helpers.cpp
        src/test/game/test_piece.cpp
        src/test/game/cands/test_capture_piece.cpp
        src/test/game/cands/test_unsafe_piece_hook.cpp
        src/test/game/cands/test_defend.cpp
        src/test/game/cands/test_development_hook.cpp
        src/test/game/cands/test_develop_piece.cpp
        src/test/game/cands/stress_test_cands.cpp
        src/test/game/cands/test_knight_fork_hook.cpp
        src/test/game/cands/test_sliding_fork_hook.cpp
        src/test/game/cands/test_queen_fork_hook.cpp
        src/test/game/cands/test_kp_forks.cpp
        src/test/game/test_weak.cpp
        src/test/game/cands/test_trade_resp.cpp
        src/test/game/cands/test_retreat_resp.cpp
        src/test/game/cands/test_desperado_resp.cpp
        src/test/game/cands/test_checks_hook.cpp

        src/test/board/test_pieces.cpp
        src/test/board/test_square.cpp
        src/test/board/test_fens.cpp)

set(TEST_MAIN src/test/test_main.cpp)
set(BENCH_MAIN src/bench/bench_main.cpp)
set(SCRATCH_MAIN scratch.cpp)
set(STASE_MAIN src/main.cpp)

include_directories(include/stase)

add_executable(stase ${STASE_MAIN} ${SOURCE_FILES} ${STASE_SOURCE_FILES} ${SOURCES})

add_executable(scratch ${SCRATCH_MAIN} ${SOURCE_FILES} ${TEST_SOURCE_FILES} ${STASE_SOURCE_FILES} ${BENCH_SOURCE_FILES} ${SCRATCH_SOURCE_FILES} ${SOURCES})

add_executable(bench ${BENCH_MAIN} ${SOURCE_FILES} ${BENCH_SOURCE_FILES} ${SOURCES})

add_executable(test ${TEST_MAIN} ${SOURCE_FILES} ${TEST_SOURCE_FILES} ${SOURCES})