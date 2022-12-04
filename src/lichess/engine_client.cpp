#include "../../include/stase/game.h"
#include "../game/gamestate.hpp"
#include "../search/engine.h"

class EngineClient {

private:
    Gamestate gs;
    int nodes;
    std::string eval_str;
    double time_elapsed;
    std::vector<Gamestate> game_history;

public:
    EngineClient() : gs(starting_pos()) {
        game_history.push_back(gs);
    }
    EngineClient(const char * fen) : gs(std::string(fen)) {
        game_history.push_back(gs);
    }

    /**
     * Fetches a move in the current position and updates the engine to the resulting position.
     * Thinks for the given time.
     */
    const char * get_computer_move(double think_time) {
#ifdef PYBIND_DEBUG_LOG
        std::cout << "[C++] entering get_computer_move\n";
#endif
        Engine engine =
            EngineBuilder::for_position(board_to_fen(gs.board))
                .with_timeout(think_time)
                .with_game_history(&game_history)
                .build();
        std::string * uci = new string(move2uci(engine.blocking_run()));
        gs = Gamestate(gs, uci2move(*uci));
        nodes = engine.get_nodes_explored();
        eval_str = etos(engine.get_score());
        time_elapsed = engine.get_actual_seconds();
        game_history.push_back(gs);
#ifdef PYBIND_DEBUG_LOG
        std::cout << "[C++] exiting get_computer_move\n";
#endif
        return uci->c_str();
    }

    /**
     * Updates the engine according to the given move (eg played by an opponent) so that
     * subsequent get_move calls reference the resulting position.
     */
    void register_opponent_move(const char * uci) {
#ifdef PYBIND_DEBUG_LOG
        std::cout << "[C++] entering register_opponent_move\n";
#endif
        gs = Gamestate(gs, uci2move(string(uci)));
        game_history.push_back(gs);
#ifdef PYBIND_DEBUG_LOG
        std::cout << "[C++] exiting register_opponent_move\n";
#endif
    }

    int get_node_count() const {
        return nodes;
    }

    double get_time_elapsed() const {
        return time_elapsed;
    }

    const char * get_eval_str() {
        return eval_str.c_str();
    }
};

extern "C" {
    EngineClient * get_client() {
        return new EngineClient();
    }
    EngineClient * get_client_for_position(const char * fen) {
        return new EngineClient(fen);
    }
    void register_opponent_move(EngineClient * client, const char * uci) {
        client->register_opponent_move(uci);
    }
    const char * get_computer_move(EngineClient * client, double seconds) {
        return client->get_computer_move(seconds);
    }
    int get_node_count(EngineClient * client) {
        return client->get_node_count();
    }
    const char * get_eval_str(EngineClient * client) {
        return client->get_eval_str();
    }
    double get_time_elapsed(EngineClient * client) {
        return client->get_time_elapsed();
    }
}
