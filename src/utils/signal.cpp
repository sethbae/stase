#include <execinfo.h>
#include "utils.h"

#include <unistd.h>

void print_stack_trace_and_abort(int sig) {

    void *array[25];
    size_t size;

    // get void*'s for all entries on the stack
    size = backtrace(array, 25);

    // print out all the frames
    fprintf(stdout, "Error: signal %d:\n", sig);
    backtrace_symbols_fd(array, size, STDOUT_FILENO);

    exit(1);
}
