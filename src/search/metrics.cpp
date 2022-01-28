#include "metrics.h"

int COUNT = 0;

void register_new_node() {
    ++COUNT;
}
int node_count() {
    return COUNT;
}
void reset_node_count() {
    COUNT = 0;
}
