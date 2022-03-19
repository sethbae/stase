#ifndef STASE_NULL_OBSERVER_HPP
#define STASE_NULL_OBSERVER_HPP

#include "search.h"

class NullObserver : public Observer {

public:
    inline void open_event(const SearchNode * node, const SearchEvent ev, const CandList *) {}
    inline void close_event(const SearchNode * node, const SearchEvent ev, const CandList *) {}

};


#endif //STASE_NULL_OBSERVER_HPP
