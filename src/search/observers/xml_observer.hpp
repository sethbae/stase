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

    inline static bool activated(SearchEvent ev) {
        switch (ev) {
            case VISIT: return true;
            case VISIT_LINE: return true;
            case SWING: return true;
            case FORCE_VISIT: return true;
            case FORCE_VISIT_LINE: return true;
            case DEEPEN: return false;
            case BURST_DEEPEN: return false;
            case BEGIN_BURST: return false;
            case UPDATE_TERMINAL: return false;
            case DEBURST: return false;
            default: return true;
        }
    }

public:

    XMLObserver(const std::string & filepath) : filepath(filepath) {}

    /**
     * Opens a new tag for the given event, and records some text on the same line. Causes a nested
     * layer in the output.
     */
    inline void open_event(const SearchNode * node, const SearchEvent ev, const CandList * cand_list, const int branch) {

        if (!activated(ev)) { return; }

        const std::string move = to_string(node) + " " +
                ((is_sentinel(node->move))
                    ? ""
                    : move2uci(node->move) + " ")
                + (cand_list ? name(*cand_list) + " " : "")
                + "(vc: " + std::to_string(node->visit_count) + ") "
                + "(q: " + std::to_string((int)quiess(*node->gs)) + ") "
                + board_to_fen(node->gs->board);
        std::string event_name = name(ev);
        if (branch) { event_name += "(" + std::to_string(branch) + ")"; }

        current_line.push_back(move2uci(node->move));
        buffer.push_back(indent(indent_level) + "<" + event_name + ">" + move);
        ++indent_level;
    }

    /**
     * Closes a previously opened tag without any other text and reduces the indentation accordingly.
     */
    inline void close_event([[maybe_unused]] const SearchNode * node, const SearchEvent ev, const CandList *, const int branch) {

        if (!activated(ev)) { return; }

        std::string branch_string =
            branch
                ? "(" + std::to_string(branch) + ")"
                : "";

        --indent_level;
        buffer.push_back(indent(indent_level) + branch_string + "</" + name(ev) + ">");
        current_line.pop_back();
    }

    /**
     * Prints a single line for the event, eg <begin_burst>0x123456789</begin_burst>
     */
    inline void register_event(const SearchNode * node, const SearchEvent ev) {
        if (!activated(ev)) { return; }
        buffer.push_back(
            indent(indent_level) + "<" + name(ev) + ">" + to_string(node) + " " + position_string() + "</" + name(ev) + ">"
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
