#include <chrono>
#include <pthread.h>
#include <thread>
#include <signal.h>
#include "engine.h"
#include "search.h"
#include "search_tools.h"

bool thread_has_stopped(pthread_t thread) {
    return pthread_kill(thread, 0) == ESRCH;
}

void * Engine::start(void * args) {

    SearchArgs * search_args = (SearchArgs *) args;

    reset_node_count();
    reset_abort_flag();

    auto start = std::chrono::high_resolution_clock::now();
    greedy_search(search_args->root, search_args->cycles, search_args->game_history, search_args->metric_weights, *search_args->o);
    auto stop = std::chrono::high_resolution_clock::now();
    double secs = duration_cast<std::chrono::microseconds>(stop - start).count() / 1000000.0;

    *search_args->nodes_out = subtree_size(search_args->root);
    *search_args->move_out = current_best_move(search_args->root);
    *search_args->score_out = search_args->root->score;
    *search_args->secs_out = secs;
    return nullptr;
}

void * Engine::start_with_timeout(void * args) {

    SearchArgs * search_args = (SearchArgs *) args;

    reset_node_count();
    reset_abort_flag();

    pthread_t t_id;
    pthread_create(
        &t_id,
        nullptr,
        &start,
        args
    );

    int n = (int) (search_args->seconds / 0.1);
    for (int i = 0; i < n; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds((int)(0.1 * 1000)));
        if (thread_has_stopped(t_id)) {
            break;
        }
    }

    abort_analysis();
    pthread_join(t_id, nullptr);
    reset_abort_flag();

    *search_args->nodes_out = subtree_size(search_args->root);
    *search_args->move_out = current_best_move(search_args->root);
    *search_args->score_out = search_args->root->score;

    return nullptr;
}

void Engine::run() {

    if (root == nullptr) { return; }

    search_args =
        new SearchArgs{
            root,
            cycle_limit,
            &obs,
            timeout_seconds,
            &nodes,
            &best_move,
            &score,
            &actual_seconds,
            game_history,
            metric_weights
        };

    set_node_limit(node_limit);

    if (timeout_seconds == -1) {
        pthread_create(
            &t_id,
            nullptr,
            &Engine::start,
            search_args
        );
    } else {
        pthread_create(
            &t_id,
            nullptr,
            &Engine::start_with_timeout,
            search_args
        );
    }
}

Move Engine::blocking_run() {
    run();
    pthread_join(t_id, nullptr);
    reset_abort_flag();
    reset_node_count();
    if (auto_cleanup) { cleanup(); }
    return best_move;
}

Move Engine::await() {
    pthread_join(t_id, nullptr);
    reset_abort_flag();
    reset_node_count();
    if (auto_cleanup) { cleanup(); }
    return best_move;
}

void Engine::kill() {
    abort_analysis();
    pthread_join(t_id, nullptr);
    reset_abort_flag();
    nodes = node_count();
    reset_node_count();
    best_move = current_best_move(root);
    if (auto_cleanup) { cleanup(); }
}

bool Engine::has_finished() {
    return thread_has_stopped(t_id);
}
