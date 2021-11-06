#include <pthread.h>
#include <string>
#include <search.h>
#include <csignal>

struct EngineParams {
    pthread_t t_id;
    SearchNode * root;
    Move best_move;
};

EngineParams current_running_config =
        {
            0,
            nullptr,
            MOVE_SENTINEL
        };

/**
 * Signal handler for being interrupted. Frees memory and records the
 * best move in the current running config.
 */
void interrupt_execution(int) {

    if (!current_running_config.root) { return; }

    current_running_config.best_move =
            current_running_config.root->best_child->move;

    delete_tree(current_running_config.root);

    pthread_exit(nullptr);

}

/**
 * Entry point for the background thread. Method will not exit - interrupt
 * with SIGINT to stop it!
 */
void * start(void *) {

    signal(SIGINT, &interrupt_execution);

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
                    new Gamestate(fen_to_board(fen)),
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
 * Cancels the given thread and fetches the best move it found.
 */
Move cancel_and_fetch_move() {

    // kill the thread and wait for it to exit
    pthread_kill(current_running_config.t_id, SIGINT);
    pthread_join(current_running_config.t_id, nullptr);

    // then return the move it selected
    return current_running_config.best_move;
}
