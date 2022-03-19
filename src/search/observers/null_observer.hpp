#ifndef STASE_NULL_OBSERVER_HPP
#define STASE_NULL_OBSERVER_HPP

#include "search.h"

class NullObserver : public Observer {

public:
    inline void open_event(const SearchNode * node, SearchEvent ev) {}
    inline void close_event(const SearchNode * node, SearchEvent ev) {}

};


#endif //STASE_NULL_OBSERVER_HPP
