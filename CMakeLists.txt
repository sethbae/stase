cmake_minimum_required(VERSION 3.13)
project(stase)
set(CMAKE_CXX_STANDARD 20)

set(SOURCE_FILES
        include/stase/board.h
        include/stase/game.h
        include/stase/search.h
        include/stase/puzzle.h

        src/board/square.h
        src/board/base_types.h
        src/board/move.h
        src/board/board.hpp
        src/board/helper.cpp
        src/board/move.cpp
        src/board/game_status.cpp

        src/game/gamestate.hpp
        src/game/eval.hpp
        src/game/control_cache.hpp
        src/game/pin_cache.hpp
        src/game/quiet.cpp

        src/game/heur/heur.cpp
        src/game/heur/heur.h
        src/game/heur/heur_util.h
        src/game/heur/piece_activity.cpp
        src/game/heur/control.cpp
        src/game/heur/pawns.cpp
        src/game/heur/development.cpp
        src/game/heur/king.cpp
        src/game/heur/endgame.cpp
        src/game/heur/mating_net.cpp

        src/puzzle/puzzle.cpp
        src/puzzle/read_formats.cpp
        src/puzzle/evaluate_rating.cpp

        src/search/search.cpp
        src/search/engine.h
        src/search/engine.cpp
        src/search/engine_client.h
        src/search/engine_client.cpp
        src/search/greedy.cpp
        src/search/metrics.cpp
        src/search/search_tools.h
        src/search/search_tools.cpp
        src/search/observers/xml_observer.hpp
        src/search/observers/observers.hpp
        src/search/board_hash.cpp
        src/search/observers/stats_observer.hpp

        src/game/cands/cands.h
        src/game/cands/score.h
        src/game/cands/hook.hpp
        src/game/cands/responder.hpp
        src/game/cands/features.hpp
        src/game/cands/cands.cpp
        src/game/cands/unsafe_piece.cpp
        src/game/cands/defend.cpp
        src/game/cands/capture_piece.cpp
        src/game/cands/develop_piece.cpp
        src/game/cands/fork.cpp
        src/game/cands/trade_retreat_desperado.cpp
        src/game/cands/checks.cpp
        src/game/cands/pin_skewer.cpp
        src/game/cands/promote.cpp
        src/game/cands/discovered.cpp
        src/game/cands/position.cpp
        src/game/cands/king.cpp

        src/game/glogic/geometry.cpp
        src/game/glogic/piece.cpp
        src/game/glogic/weak.cpp
        src/game/glogic/king.cpp
        src/game/glogic/glogic.h
        src/game/glogic/piece_moves.hpp
        src/game/glogic/edge_of_board_lookup.h
        src/game/glogic/piece_encounter_cache.hpp
        src/game/glogic/piece_encounter_lookups.cpp
        src/game/glogic/pawn.cpp
        src/game/glogic/control.cpp
        src/game/glogic/square_control.cpp
        src/game/king_net.hpp

        src/utils/utils.h
        src/utils/print.cpp
        src/utils/uci.cpp
        src/utils/signal.cpp
        src/utils/ptr_vec.h src/search/engine_client.h)

set(STASE_SOURCE_FILES)

set(SCRATCH_SOURCE_FILES)

set(TEST_SOURCE_FILES

        src/test/test.h
        src/test/test_helpers.cpp
        src/test/stress_test_main.cpp

        src/test/game/cands/stress_test_cands.cpp

        src/test/game/test_game.cpp
        src/test/game/test_gs_piece_lists.cpp
        src/test/game/test_cands_sorting.cpp

        src/test/game/cands/test_capture_piece.cpp
        src/test/game/cands/test_unsafe_piece_hook.cpp
        src/test/game/cands/test_defend.cpp
        src/test/game/cands/test_development_hook.cpp
        src/test/game/cands/test_develop_piece.cpp
        src/test/game/cands/fork_helpers.h
        src/test/game/cands/test_knight_fork_hook.cpp
        src/test/game/cands/test_sliding_fork_hook.cpp
        src/test/game/cands/test_queen_fork_hook.cpp
        src/test/game/cands/test_kp_forks.cpp
        src/test/game/cands/test_trade_resp.cpp
        src/test/game/cands/test_retreat_resp.cpp
        src/test/game/cands/test_desperado_resp.cpp
        src/test/game/cands/test_checks_hook.cpp
        src/test/game/cands/test_pin_skewer_hook.cpp
        src/test/game/cands/test_king_pinned_pieces_hook.cpp
        src/test/game/cands/test_promotion.cpp
        src/test/game/cands/test_fork_responder.cpp
        src/test/game/cands/test_discovered_hook.cpp
        src/test/game/cands/test_approach_kings.cpp

        src/test/game/glogic/test_weak.cpp
        src/test/game/glogic/test_piece.cpp
        src/test/game/glogic/test_luft.cpp

        src/test/board/test_board.cpp
        src/test/board/test_pieces.cpp
        src/test/board/test_square.cpp
        src/test/board/test_fens.cpp
        src/test/board/test_in_check.cpp
        src/test/board/test_castling.cpp
        src/test/board/test_mutate.cpp
        src/test/board/test_move.cpp
        src/test/board/test_game_status.cpp

        src/test/integration/integration_test.h
        src/test/integration/integration_test_main.cpp
        src/test/integration/test_piece_mates.cpp

        src/test/search/test_search.cpp
        src/test/search/test_search_helpers.h
        src/test/search/test_cand_list_ordering.cpp
        src/test/search/test_cand_list_visit_counts.cpp
        src/test/search/test_legal_moves_present.cpp
        src/test/search/test_terminal.cpp
        src/test/search/test_best_child.cpp
        src/test/search/test_search_helpers.cpp
        src/test/search/test_past_blunders.cpp
        src/test/search/test_past_crashes.cpp
        src/test/search/test_parent_pointer.cpp
        src/test/search/test_checkmates.cpp)

set(TEST_MAIN src/test/test_main.cpp)
set(SCRATCH_MAIN scratch.cpp)
set(STASE_MAIN src/main.cpp)

include_directories(include/stase)

add_executable(stase ${STASE_MAIN} ${SOURCE_FILES} ${STASE_SOURCE_FILES} ${SOURCES})

add_executable(scratch ${SCRATCH_MAIN} ${SOURCE_FILES} ${TEST_SOURCE_FILES} ${STASE_SOURCE_FILES} ${SCRATCH_SOURCE_FILES} ${SOURCES})

add_executable(test ${TEST_MAIN} ${SOURCE_FILES} ${TEST_SOURCE_FILES} ${SOURCES})

add_library(engine_client
            SHARED
            ${SOURCE_FILES} src/search/engine_client.cpp)

# these two lines are apparently a workaround which force the library to be rebuilt whenever
# the sources change.
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR})
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})


SET(CMAKE_CXX_FLAGS "-pthread -rdynamic ${CMAKE_CXX_FLAGS}")

if(DEBUG)
    SET(CMAKE_CXX_FLAGS "-Ofast -DSAFE_STACK_VEC -DPYBIND_DEBUG_LOG -DENGINE_STACK_TRACE -g -Wall -Wextra -Wno-sign-compare -Werror -fsanitize=address -fsanitize=undefined ${CMAKE_CXX_FLAGS}")
else()
    SET(CMAKE_CXX_FLAGS "-Ofast -fno-exceptions -DENGINE_STACK_TRACE ${CMAKE_CXX_FLAGS}")
endif()
