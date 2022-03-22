#ifndef STASE_THREAD_H
#define STASE_THREAD_H

#include "search.h"

void abort_analysis();
bool abort_flag();
void reset_abort_flag();

void set_node_limit(int nodes);
int get_node_limit();
void clear_node_limit();

/**
 * Returns true iff there is no running engine thread. Callers should still go on to call
 * stop_engine before exiting or starting another.
 */
bool engine_has_stopped();

void interrupt_execution(int);

#endif //STASE_THREAD_H
