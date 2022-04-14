#include "../../include/stase/game.h"
#include "../game/gamestate.hpp"
#include "../search/engine.h"

class EngineClient {

private:
    Gamestate gs;

public:
    EngineClient() : gs(starting_pos()) {}
    EngineClient(const char * fen) : gs(std::string(fen)) {}

    const char * get_move(double think_time) {
        Engine engine =
            EngineBuilder::for_position(board_to_fen(gs.board))
                .with_timeout(think_time)
                .build();
        return (new string(move2uci(engine.blocking_run())))->c_str();
    }

    void advance_position(const char * uci) {
        gs.next_in_place(uci2move(string(uci)));
    }
};

extern "C" {
    EngineClient * get_client() {
        return new EngineClient();
    }
    EngineClient * get_client_for_position(const char * fen) {
        return new EngineClient(fen);
    }
    void advance_position(EngineClient * client, const char * uci) {
        client->advance_position(uci);
    }
    const char * get_move(EngineClient * client, double seconds) {
        return client->get_move(seconds);
    }
}
