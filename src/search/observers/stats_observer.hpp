#ifndef STASE_STATS_OBSERVER_HPP
#define STASE_STATS_OBSERVER_HPP

#include "search.h"

class StatsObserver : public Observer {
private:
    int * counts;
public:
    StatsObserver() {
        counts = new int[ALL_SEARCH_EVENTS.size()];
        for (int i = 0; i < 8; ++i) {
            counts[i] = 0;
        }
    }

    void open_event(const SearchNode *, const SearchEvent search_event, const CandList *, const int) {
        ++counts[search_event];
    }

    void register_event(const SearchNode *, const SearchEvent search_event) {
        ++counts[search_event];
    }

    void print_summary(std::ostream & o) const {
        o << "StatsObserver saw these events:\n";
        for (const SearchEvent ev : ALL_SEARCH_EVENTS) {
            o << std::setw(20) << name(ev) << ": " << counts[ev] << "\n";
        }
    }
};

#endif //STASE_STATS_OBSERVER_HPP
