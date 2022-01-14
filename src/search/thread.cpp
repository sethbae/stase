#include <pthread.h>
#include <string>
#include <search.h>
#include "metrics.h"
#include "thread.h"

struct EngineParams {
    pthread_t t_id;
    SearchNode * root;
    Move best_move;
    int nodes;
};

EngineParams current_running_config =
        {
            0,
            nullptr,
            MOVE_SENTINEL,
            0
        };

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
    search_indefinite(current_running_config.root);

    return nullptr;
}

/**
 * Starts analysing the given fen in the background. Returns a handle to the thread used
 * which can be used to cancel it later.
 */
void run_in_background(const std::string & fen) {

    current_running_config.root =
            new SearchNode{
                    new Gamestate(fen),
                    new CandSet,
                    zero(),
                    MOVE_SENTINEL,
                    0,
                    nullptr,
                    nullptr
            };

    pthread_t t_id;

    pthread_create(
            &t_id,
            nullptr,
            &start,
            nullptr
    );

    current_running_config.t_id = t_id;
}

/**
 * Cancels the given thread and fetches the best move it found. Responsible for cleaning up
 * the memory the engine was using etc.
 */
void stop_engine(bool cleanup) {

    // kill the thread and wait for it to exit
    abort_analysis();
    pthread_join(current_running_config.t_id, nullptr);

    // retrieve info from the run
    if (current_running_config.root) {
        current_running_config.nodes = node_count();
        current_running_config.best_move = current_best_move(current_running_config.root);
    }

    // and delete the search tree
    if (cleanup) { delete_tree(current_running_config.root); }

    reset_abort_flag();
}

/**
 * Returns the best move which the engine found. This should only be called after the
 * engine has been stopped - values returned while the engine are running are not
 * likely to be meaningful.
 */
Move fetch_best_move() {
    return current_running_config.best_move;
}

/**
 * Returns the number of nodes explored by the engine. As with the best move, this is
 * only safe to use after the engine has been stopped.
 */
int fetch_node_count() {
    return current_running_config.nodes;
}

SearchNode * fetch_root() {
    return current_running_config.root;
}
