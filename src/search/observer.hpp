#include "search.h"

enum SearchEvent {
    VISIT,
    VISIT_LINE,
    FORCE_VISIT,
    FORCE_VISIT_LINE,
    DEEPEN
};

class NullObserver {
public:
    inline void open_event(const SearchNode * node, SearchEvent ev) {}
    inline void close_event(const SearchNode * node, SearchEvent ev) {}
};


class Observer {

private:
    const std::string filepath;
    std::vector<std::string> current_line;
    int indent_level = 0;

public:

    Observer(const std::string & filepath) : filepath(filepath) {}

    inline void open_event(const SearchNode * node, SearchEvent ev) {}
    inline void close_event(const SearchNode * node, SearchEvent ev) {}

};
