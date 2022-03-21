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
    // we count indents from 1 so that the <main> tag can sit at the global level
    int indent_level = 1;

public:

    XMLObserver(const std::string & filepath) : filepath(filepath) {}

    /**
     * Opens a new tag for the given event, and records some text on the same line. Causes a nested
     * layer in the output.
     */
    inline void open_event(const SearchNode * node, const SearchEvent ev, const CandList * cand_list) {
        const std::string move = to_string(node) + " " +
                ((is_sentinel(node->move))
                    ? ""
                    : move2uci(node->move) + " ")
                + (cand_list ? name(*cand_list) : "");
        current_line.push_back(move2uci(node->move));
        buffer.push_back(indent(indent_level) + "<" + name(ev) + ">" + move);
        ++indent_level;
    }

    /**
     * Closes a previously opened tag without any other text and reduces the indentation accordingly.
     */
    inline void close_event(const SearchNode * node, const SearchEvent ev, const CandList *) {
        --indent_level;
        buffer.push_back(indent(indent_level) + "</" + name(ev) + ">");
        current_line.pop_back();
    }

    /**
     * Prints a single line for the event, eg <begin_burst>0x123456789</begin_burst>
     */
    inline void register_event(const SearchNode * node, const SearchEvent ev) {
        buffer.push_back(
            indent(indent_level) + "<" + name(ev) + ">" + to_string(node) + "</" + name(ev) + ">"
        );
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
