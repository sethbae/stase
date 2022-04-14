#include "../../include/stase/game.h"
#include "../game/gamestate.hpp"
#include "../search/engine.h"

class EngineClient {

private:
    Gamestate gs;

public:
    EngineClient() : gs(starting_pos()) {}
    EngineClient(const char * fen) : gs(std::string(fen)) {}

    /**
     * Fetches a move in the current position and updates the engine to the resulting position.
     * Thinks for the given time.
     */
    const char * get_computer_move(double think_time) {
        Engine engine =
            EngineBuilder::for_position(board_to_fen(gs.board))
                .with_timeout(think_time)
                .build();
        std::string * uci = new string(move2uci(engine.blocking_run()));
        gs = Gamestate(gs, uci2move(*uci));
        return uci->c_str();
    }

    /**
     * Updates the engine according to the given move (eg played by an opponent) so that
     * subsequent get_move calls reference the resulting position.
     */
    void register_opponent_move(const char * uci) {
        gs = Gamestate(gs, uci2move(string(uci)));
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
}
