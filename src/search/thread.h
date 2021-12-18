#ifndef STASE_THREAD_H
#define STASE_THREAD_H

#include "search.h"

void abort_analysis();
void reset_abort_flag();
void check_abort();

void interrupt_execution(int);

#endif //STASE_THREAD_H
