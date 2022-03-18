#include <fstream>
#include "search.h"
#include "../utils/utils.h"

enum SearchEvent {
    VISIT,
    VISIT_LINE,
    FORCE_VISIT,
    FORCE_VISIT_LINE,
    DEEPEN
};

inline std::string name(SearchEvent ev) {
    switch (ev) {
        case VISIT: return "visit";
        case VISIT_LINE: return "visit_l";
        case FORCE_VISIT: return "f_visit";
        case FORCE_VISIT_LINE: return "f_visit_l";
        case DEEPEN: return "deepen";
        default: return "unknown event";
    }
}

class NullObserver {
public:
    inline void open_event(const SearchNode * node, SearchEvent ev) {}
    inline void close_event(const SearchNode * node, SearchEvent ev) {}
};


class Observer {

private:
    const std::string filepath;
    std::vector<std::string> current_line;
    std::vector<std::string> buffer;
    int indent_level = 1;
    const SearchNode * parent = nullptr;

public:

    Observer(const std::string & filepath) : filepath(filepath) {}

    inline void open_event(const SearchNode * node, SearchEvent ev) {
        const std::string move = (is_sentinel(node->move))
                ? ""
                : '"' + move2uci(node->move) + '"';
        current_line.push_back(move2uci(node->move));
        buffer.push_back(indent(indent_level) + "<" + name(ev) + ">" + move);
        ++indent_level;
        parent = node;
    }

    inline void close_event(const SearchNode * node, SearchEvent ev) {
        const std::string move = (is_sentinel(node->move))
                 ? ""
                 : '"' + move2uci(node->move) + '"';
        --indent_level;
        buffer.push_back(indent(indent_level) + "</" + name(ev) + ">" + move);
        current_line.pop_back();
    }

    inline void write() {
        std::ofstream file;
        file.open(filepath, std::ios::out);
        file << "<main>\n";
        for (const std::string & line : buffer) {
            file << (line + "\n");
        }
        file << "</main>\n";
        file.close();
    }

private:

    inline std::string content(const SearchNode * node, SearchEvent ev) {
        if (ev == DEEPEN) {
            return "";
        }
    }

    inline std::string position_string() {
        std::string s = "";
        for (int i = 0; i < current_line.size() - 1; ++i) {
            s += current_line[i];
            s += " ";
        }
        s += current_line[current_line.size() - 1];
        return s;
    }

    inline std::string indent(const int n) {
        std::string s = "";
        for (int i = 0; i < n; ++i) {
            s += " ";
        }
        return s;
    }
};
