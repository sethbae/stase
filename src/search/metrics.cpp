#include "search_tools.h"

volatile int COUNT = 0;

void register_new_node() {
    COUNT = COUNT + 1;
}
int node_count() {
    return COUNT;
}
void reset_node_count() {
    COUNT = 0;
}
