#include "search.h"

class Observer {

private:
    const std::string filepath;
    std::vector<std::string> current_line;
    int indent_level = 0;

public:

    Observer(const std::string & filepath) : filepath(filepath) {}

    void open_visit(const SearchNode * node) {}
    void close_visit(const SearchNode * node) {}
    void open_deepen(const SearchNode * node) {}
    void close_deepen(const SearchNode * node) {}

};
