#include "engine_client.h"
#include "engine.h"
#include "../game/gamestate.hpp"

EngineClient::EngineClient() :
        gs(new Gamestate(Board::starting_pos())),
        nodes(0),
        game_status(ONGOING)
{
    game_history.push_back(*gs);
}

EngineClient::EngineClient(const char * fen) :
        gs(new Gamestate(std::string(fen))),
        nodes(0),
        game_status(ONGOING)
{
    game_history.push_back(*gs);
}

EngineClient::EngineClient(const std::string & fen) :
        gs(new Gamestate(fen)),
        nodes(0),
        game_status(ONGOING)
{
    game_history.push_back(*gs);
}

EngineClient::EngineClient(const std::string & fen, GamePhase phase) :
        gs(new Gamestate(fen, phase)),
        nodes(0),
        game_status(ONGOING)
{
    game_history.push_back(*gs);
}

const char * EngineClient::get_computer_move(double think_time) {
#ifdef PYBIND_DEBUG_LOG
    std::cout << "[C++] entering get_computer_move\n";
#endif
    if (game_has_ended()) {
        return (new std::string(name(game_status)))->c_str();
    }
    Engine engine =
        EngineBuilder::for_position(board_utils::board_to_fen(gs->board))
            .with_timeout(think_time)
            .with_game_history(&game_history)
            .build();
    std::string * uci = new string(move2uci(engine.blocking_run()));
    gs = new Gamestate(*gs, uci2move(*uci));
    nodes = engine.get_nodes_explored();
    eval_str = etos(engine.get_score());
    game_history.push_back(*gs);
    update_status();
#ifdef PYBIND_DEBUG_LOG
    std::cout << "[C++] exiting get_computer_move\n";
#endif
    return uci->c_str();
}

void EngineClient::register_opponent_move(const std::string & uci) {
    register_opponent_move(uci.c_str());
}

void EngineClient::register_opponent_move(const char * uci) {
#ifdef PYBIND_DEBUG_LOG
    std::cout << "[C++] entering register_opponent_move\n";
#endif
    gs = new Gamestate(*gs, uci2move(string(uci)));
    game_history.push_back(*gs);
    update_status();
#ifdef PYBIND_DEBUG_LOG
    std::cout << "[C++] exiting register_opponent_move\n";
#endif
}

int EngineClient::get_node_count() const {
    return nodes;
}

const char * EngineClient::get_eval_cstr() {
    return eval_str.c_str();
}

std::string EngineClient::get_eval_str() {
    return eval_str;
}

bool EngineClient::game_has_ended() const {
    return game_status != ONGOING;
}

GameStatus EngineClient::current_status() const {
    return game_status;
}

void EngineClient::update_status() {
    std::vector<Board> boards(game_history.size());
    for (const Gamestate & gamestate : game_history) {
        boards.push_back(gamestate.board);
    }
    game_status = game_rules::check_status(boards);
}

/**
 * The Lichess driver uses cython to interface with the engine. These declare the functions used by
 * the python EngineClient.
 */
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
        return client->get_eval_cstr();
    }
}
