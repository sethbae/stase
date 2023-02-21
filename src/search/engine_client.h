#ifndef STASE_ENGINE_CLIENT_H
#define STASE_ENGINE_CLIENT_H

#include "../../include/stase/game.h"

/**
 * THe EngineClient provides an easy interface to play a game involving an engine. Moves can be fetched and opponent
 * moves can be added. The game-history is tracked so that 3-fold repetition is correctly accounted for.
 */
class EngineClient {
    Gamestate * gs;
    int nodes;
    std::string eval_str;
    std::vector<Gamestate> game_history;
    GameStatus game_status;

public:
    EngineClient();
    explicit EngineClient(const char * fen);
    explicit EngineClient(const std::string & fen);
    explicit EngineClient(const std::string & fen, GamePhase phase);

    /**
     * Fetches a move in the current position and updates the engine to the resulting position.
     * Thinks for the given time.
     */
    const char * get_computer_move(double think_time);

    /**
     * Updates the engine according to the given move (eg played by an opponent) so that
     * subsequent get_move calls reference the resulting position.
     */
    void register_opponent_move(const char * uci);

    void register_opponent_move(const std::string & uci);

    int get_node_count() const;

    const char * get_eval_cstr();

    std::string get_eval_str();

    bool game_has_ended() const;

    GameStatus current_status() const;

private:
    void update_status();
};

#endif //STASE_ENGINE_CLIENT_H
