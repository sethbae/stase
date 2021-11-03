#include <pthread.h>
#include <string>
#include <search.h>
#include <iostream>
#include "unistd.h"

SearchNode * most_recent_root;

struct EngineParams {
    SearchNode * root;
};

void * start(void * arg) {

    EngineParams * params = (EngineParams *) arg;

    search_indefinite(params->root);

    return nullptr;
}

/**
 * Starts analysing the given fen in the background. Returns a handle to the thread used
 * which can be used to cancel it later.
 */
pthread_t run_in_background(const std::string & fen) {

    delete_tree(most_recent_root);

    most_recent_root =
            new SearchNode{
                    new Gamestate(fen_to_board(fen)),
                    zero(),
                    MOVE_SENTINEL,
                    0,
                    nullptr,
                    nullptr
            };

    EngineParams * params = new EngineParams{most_recent_root};

    pthread_t t_id;

    pthread_create(
            &t_id,
            nullptr,
            &start,
            params
    );

    return t_id;
}

/**
 * Cancels the given thread and fetches the best move it found.
 */
Move cancel_and_fetch_move(pthread_t t_id) {
    pthread_cancel(t_id);
    return most_recent_root->best_child->move;
}

/**
 * Cleans up the search tree (if any) created by the background thread.
 */
void free_memory_for_exit() {
    if (most_recent_root) {
        delete_tree(most_recent_root);
    }
}
