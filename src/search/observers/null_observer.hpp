#ifndef STASE_NULL_OBSERVER_HPP
#define STASE_NULL_OBSERVER_HPP

#include "search.h"

class NullObserver : public Observer {

public:
    inline void open_event(const SearchNode *, const SearchEvent, const CandList *) {}
    inline void close_event(const SearchNode *, const SearchEvent, const CandList *) {}
    inline void register_event(const SearchNode *, const SearchEvent) {}

};


#endif //STASE_NULL_OBSERVER_HPP
