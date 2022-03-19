#ifndef STASE_XML_OBSERVER_HPP
#define STASE_XML_OBSERVER_HPP

#include <fstream>
#include "search.h"
#include "../../utils/utils.h"


class XMLObserver : public Observer {

private:
    const std::string filepath;
    std::vector<std::string> current_line;
    std::vector<std::string> buffer;
    int indent_level = 1;

public:

    XMLObserver(const std::string & filepath) : filepath(filepath) {}

    inline void open_event(const SearchNode * node, const SearchEvent ev, const CandList *) {
        const std::string move = (is_sentinel(node->move))
                ? ""
                : '"' + move2uci(node->move) + '"';
        current_line.push_back(move2uci(node->move));
        buffer.push_back(indent(indent_level) + "<" + name(ev) + ">" + move);
        ++indent_level;
    }

    inline void close_event(const SearchNode * node, const SearchEvent ev, const CandList *) {
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

#endif // STASE_XML_OBSERVER_HPP
