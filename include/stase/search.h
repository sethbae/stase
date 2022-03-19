#ifndef STASE_SEARCH_H
#define STASE_SEARCH_H

#include "board.h"
#include "game.h"

struct SearchNode {

    Gamestate * gs;
    CandSet * cand_set;
    Eval score;
    Move move;
    std::vector<SearchNode*> children;
    SearchNode * best_child;
    SearchNode * best_trust_child;
    unsigned short visit_count;

};

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

class Observer {

public:
    virtual void open_event(const SearchNode *, const SearchEvent, const CandList * = nullptr) = 0;
    virtual void close_event(const SearchNode *, const SearchEvent, const CandList * = nullptr) = 0;

};

std::vector<Move> greedy_search(const std::string &, int, Observer &);
std::vector<Move> greedy_search(SearchNode *, int, Observer &);

void search_indefinite(SearchNode *, Observer * = nullptr);
void delete_tree(SearchNode *);

int subtree_size(SearchNode *);
int subtree_depth(SearchNode *);
void write_to_file(SearchNode*, std::ostream &);
void write_to_file_recursively(SearchNode*, std::ostream &);

void run_in_background(const std::string & fen, Observer * = nullptr);
void run_with_node_limit(const std::string & fen, int, Observer * = nullptr);

void stop_engine(bool cleanup=true);
Move fetch_best_move();
int fetch_node_count();
SearchNode * fetch_root();

#endif //STASE_SEARCH_H
