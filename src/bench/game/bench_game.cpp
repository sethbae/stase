#include "../bench.h"

int bench_game() {

    bench_individual_metrics();

    cout << "\n";

    bench_heuristic_evaluation();

    cout << "\n";

    bench_individual_hooks();

    cout << "\n";

    bench_individual_responders();

    cout << "\n";

    bench_cands();

    return 0;

}
