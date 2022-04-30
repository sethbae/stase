#ifndef STASE_ENGINE_H
#define STASE_ENGINE_H

#include "../../include/stase/search.h"
#include "../../include/stase/game.h"
#include "../game/gamestate.hpp"

class Engine {

private:

    struct SearchArgs{
        SearchNode * root;
        int cycles;
        Observer * o;
        double seconds;
        int * nodes_out;
        Move * move_out;
        Eval * score_out;
    };

    const std::string fen;
    SearchNode * root;
    Observer & obs;
    SearchArgs * search_args;

    pthread_t t_id;
    int node_limit;
    int cycle_limit;
    double timeout_seconds;
    bool cleanup;

    Move best_move;
    int nodes;
    Eval score;

public:
    Engine(const std::string fen, Observer & o, int node_limit, int cycle_limit, double timeout_seconds, bool cleanup):
        fen(fen),
        obs(o),
        search_args(nullptr),

        t_id(0),
        node_limit(node_limit),
        cycle_limit(cycle_limit),
        timeout_seconds(timeout_seconds),
        cleanup(cleanup),

        best_move(MOVE_SENTINEL),
        nodes(0)
    {
        root = new SearchNode{
            new Gamestate(fen),
            new CandSet,
            zero(),
            MOVE_SENTINEL,
            false,
            {},
            nullptr,
            nullptr,
            0
        };
    }

    ~Engine() {
        delete search_args;
    }

    Observer & get_obs() const { return obs; }
    std::string get_fen() const { return fen; }
    SearchNode * get_root() const { return root; }

    void run();
    Move blocking_run();
    Move await();
    void kill();

    bool has_finished();

    Move get_best_move() { return best_move; }
    int get_nodes_explored() { return nodes; }
    Eval get_score() { return score; }
    void delete_tree() { ::delete_tree(root); root = nullptr; }

private:
    static void * start(void *);
    static void * start_with_timeout(void *);
};


class EngineBuilder {

private:
    std::string fen;
    Observer & obs;
    int nodes;
    int cycles;
    double seconds;
    bool cleanup;

    EngineBuilder(std::string fen, Observer & obs, int node_limit, int cycle_limit, double timeout_seconds, bool cleanup) :
        fen(fen),
        obs(obs),
        nodes(node_limit),
        cycles(cycle_limit),
        seconds(timeout_seconds),
        cleanup(cleanup)
    {}

public:

    static EngineBuilder for_position(std::string fen_string) {
        return EngineBuilder(fen_string, DEFAULT_OBSERVER, -1, -1, -1, true);
    }

    static EngineBuilder for_starting_position() {
        return for_position(std::string(starting_fen()));
    }

    EngineBuilder with_obs(Observer & observer) {
        return EngineBuilder(fen, observer, nodes, cycles, seconds, cleanup);
    }

    EngineBuilder with_node_limit(int node_limit) {
        return EngineBuilder(fen, obs, node_limit, cycles, seconds, cleanup);
    }

    EngineBuilder with_cycle_limit(int cycle_limit) {
        return EngineBuilder(fen, obs, nodes, cycle_limit, seconds, cleanup);
    }

    EngineBuilder with_timeout(double secs) {
        return EngineBuilder(fen, obs, nodes, cycles, secs, cleanup);
    }

    EngineBuilder with_cleanup(bool auto_cleanup) {
        return EngineBuilder(fen, obs, nodes, cycles, seconds, auto_cleanup);
    }

    Engine build() {
        return Engine(fen, obs, nodes, cycles, seconds, cleanup);
    }
};

#endif //STASE_ENGINE_H
