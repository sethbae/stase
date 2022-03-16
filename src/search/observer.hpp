#include "search.h"

class NullObserver {
public:
    inline void open_visit(const SearchNode * node) {}
    inline void close_visit(const SearchNode * node) {}
    inline void open_visit_line(const SearchNode * node) {}
    inline void close_visit_line(const SearchNode * node) {}
    inline void open_force_visit(const SearchNode * node) {}
    inline void close_force_visit(const SearchNode * node) {}
    inline void open_force_visit_line(const SearchNode * node) {}
    inline void close_force_visit_line(const SearchNode * node) {}
    inline void open_deepen(const SearchNode * node) {}
    inline void close_deepen(const SearchNode * node) {}
};


class Observer {

private:
    const std::string filepath;
    std::vector<std::string> current_line;
    int indent_level = 0;

public:

    Observer(const std::string & filepath) : filepath(filepath) {}

    inline void open_visit(const SearchNode * node) {}
    inline void close_visit(const SearchNode * node) {}
    inline void open_visit_line(const SearchNode * node) {}
    inline void close_visit_line(const SearchNode * node) {}
    inline void open_force_visit(const SearchNode * node) {}
    inline void close_force_visit(const SearchNode * node) {}
    inline void open_force_visit_line(const SearchNode * node) {}
    inline void close_force_visit_line(const SearchNode * node) {}
    inline void open_deepen(const SearchNode * node) {}
    inline void close_deepen(const SearchNode * node) {}

};
