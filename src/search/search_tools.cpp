#include <iostream>
#include "search_tools.h"
#include "thread.h"
#include "metrics.h"

/**
 * Checks whether the engine abort flag has been set and gracefully exits if so.
 */
void check_abort() {
    std::cout << node_count() << " " << get_node_limit() << "\n";
    if (abort_flag() || node_count() >= get_node_limit()) {
        interrupt_execution(0);
    }
}
