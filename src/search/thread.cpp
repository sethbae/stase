#include <pthread.h>
#include <thread>
#include <chrono>
#include <signal.h>
#include <string>
#include <search.h>
#include "metrics.h"
#include "thread.h"
#include "../game/gamestate.hpp"

class EngineConfiguration {

public:
    pthread_t t_id;
    SearchNode * root;
    Observer & obs;
    Move best_move;
    int nodes;

    EngineConfiguration():
        t_id(0),
        root(nullptr),
        obs(DEFAULT_OBSERVER),
        best_move(MOVE_SENTINEL),
        nodes(0)
    {}

    EngineConfiguration(const std::string & fen, Observer & o):
        t_id(0),
        obs(o),
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

};

EngineConfiguration * current_running_config = nullptr;

/**
 * Interrupts the calling thread. Immediately exits with no cleanup.
 */
void interrupt_execution(int) {
    pthread_exit(nullptr);
}

/**
 * Entry point for the background thread. Stop using abort_analysis().
 */
void * start(void *) {

    reset_node_count();
    reset_abort_flag();
    search_indefinite(current_running_config->root, current_running_config->obs);

    return nullptr;
}

/**
 * Starts analysing the given fen in the background. Returns a handle to the thread used
 * which can be used to cancel it later.
 */
void run_in_background(const std::string & fen, Observer & obs) {

    delete current_running_config;
    current_running_config = new EngineConfiguration(fen, obs);

    pthread_t t_id;

    pthread_create(
        &t_id,
        nullptr,
        &start,
        nullptr
    );

    current_running_config->t_id = t_id;
}

void run_with_node_limit(const std::string & fen, int node_limit, Observer & obs) {
    set_node_limit(node_limit);
    run_in_background(fen, obs);
    pthread_join(current_running_config->t_id, nullptr);
    clear_node_limit();
}

/**
 * Runs the engine in a blocking fashion. Waits for at most [time_out_seconds] seconds, checking every
 * [poll_every_seconds] whether the engine has exited early or not. If so, kills the engine and returns
 * the best move. Cleanup is performed when killing the engine according to the flag [cleanup].
 */
Move run_with_timeout(const std::string & fen, double time_out_seconds, double poll_every_seconds, bool cleanup) {

    run_in_background(fen);

    int n = (int) (time_out_seconds / poll_every_seconds);
    for (int i = 0; i < n; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds((int)(poll_every_seconds * 1000)));
        if (engine_has_stopped()) {
            break;
        }
    }

    stop_engine(cleanup);
    return fetch_best_move();
}

/**
 * Cancels the given thread and fetches the best move it found. Responsible for cleaning up
 * the memory the engine was using etc.
 */
void stop_engine(bool cleanup) {

    // kill the thread and wait for it to exit
    abort_analysis();
    pthread_join(current_running_config->t_id, nullptr);
    reset_abort_flag();

    // retrieve info from the run
    if (current_running_config->root) {
        current_running_config->nodes = node_count();
        current_running_config->best_move = current_best_move(current_running_config->root);
    }

    // and delete the search tree
    if (cleanup) { delete_tree(current_running_config->root); }

}

/**
 * Returns the best move which the engine found. This should only be called after the
 * engine has been stopped - values returned while the engine are running are not
 * likely to be meaningful.
 */
Move fetch_best_move() {
    return current_running_config->best_move;
}

/**
 * Returns the number of nodes explored by the engine. As with the best move, this is
 * only safe to use after the engine has been stopped.
 */
int fetch_node_count() {
    return current_running_config->nodes;
}

SearchNode * fetch_root() {
    return current_running_config->root;
}

/**
 * Returns true iff there is no running engine thread. Callers should still go on to call
 * stop_engine before exiting or starting another.
 */
bool engine_has_stopped() {
    // in order to check whether the thread is still running, you can send a kill signal
    // with zero as the signo. This is intended for error checking: if the thread has
    // exited, pthread_kill won't be able to find and kill it, and so returns ESRCH (Error SeaRCHing)
    return current_running_config && (pthread_kill(current_running_config->t_id, 0) == ESRCH);
}

bool engine_abort = false;
bool abort_flag() { return engine_abort; }
void abort_analysis() { engine_abort = true; }
void reset_abort_flag() { engine_abort = false; }

int max_nodes = -1;
int get_node_limit() { return max_nodes; }
void set_node_limit(int limit) { max_nodes = limit; }
void clear_node_limit() { max_nodes = -1; }
