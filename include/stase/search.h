#ifndef STASE_SEARCH_H
#define STASE_SEARCH_H

#include "board.h"
#include "game.h"

struct SearchNode {

    Gamestate * gs;
    CandSet * cand_set;
    Eval score;
    Move move;
    bool terminal;
    long board_hash;
    const SearchNode * parent;
    std::vector<SearchNode*> children;
    SearchNode * best_child;
    SearchNode * best_trust_child;
    unsigned short visit_count;

    SearchNode(Gamestate * gs, CandSet * cand_set) :
        gs(gs),
        cand_set(cand_set),
        score(zero()),
        move(MOVE_SENTINEL),
        terminal(false),
        board_hash(hash()),
        parent(nullptr),
        children(),
        best_child(nullptr),
        best_trust_child(nullptr),
        visit_count(0)
    {}

    SearchNode(Gamestate * gs, CandSet * cand_set, Move m) :
        gs(gs),
        cand_set(cand_set),
        score(zero()),
        move(m),
        terminal(false),
        board_hash(hash()),
        parent(nullptr),
        children(),
        best_child(nullptr),
        best_trust_child(nullptr),
        visit_count(0)
    {}

    SearchNode(Gamestate * gs, CandSet * cand_set, Eval e) :
        gs(gs),
        cand_set(cand_set),
        score(e),
        move(MOVE_SENTINEL),
        terminal(false),
        board_hash(hash()),
        parent(nullptr),
        children(),
        best_child(nullptr),
        best_trust_child(nullptr),
        visit_count(0)
    {}

private: long hash();
};

enum SearchEvent {
    VISIT,
    VISIT_LINE,
    SWING,
    FORCE_VISIT,
    FORCE_VISIT_LINE,
    DEEPEN,
    BURST_DEEPEN,
    BEGIN_BURST,
    UPDATE_TERMINAL,
    DEBURST
};

inline std::string name(SearchEvent ev) {
    switch (ev) {
        case VISIT: return "visit";
        case VISIT_LINE: return "visit_l";
        case SWING: return "swing";
        case FORCE_VISIT: return "f_visit";
        case FORCE_VISIT_LINE: return "f_visit_l";
        case DEEPEN: return "deepen";
        case BURST_DEEPEN: return "burst_deepen";
        case BEGIN_BURST: return "begin_burst";
        case UPDATE_TERMINAL: return "update_terminal";
        case DEBURST: return "deburst";
        default: return "unknown event";
    }
}

/**
 * The Observer class can be used to monitor a search at runtime. Potential applications
 * include recording the order of paths explored (XMLObserver) or various correctness tests.
 * It is an abstract class whose methods can be selectively overridden as required.
 */
class Observer {

public:
    virtual void open_event(const SearchNode *, const SearchEvent, const CandList * = nullptr, const int = 0) {}
    virtual void close_event(const SearchNode *, const SearchEvent, const CandList * = nullptr, const int = 0) {}
    virtual void register_event(const SearchNode *, const SearchEvent) {}

};

/**
 * The default observer should be used when an observer is not required: it does nothing and
 * should get optimised away with no performance cost at all.
 */
extern Observer DEFAULT_OBSERVER;

std::vector<Move> greedy_search(const std::string &, int, Observer & = DEFAULT_OBSERVER);
std::vector<Move> greedy_search(SearchNode *, int, Observer & = DEFAULT_OBSERVER);

void delete_tree(SearchNode *);

int subtree_size(SearchNode *);
int subtree_depth(SearchNode *);
void write_to_file(SearchNode *, std::ostream &);
void write_to_file_recursively(SearchNode*, std::ostream &);

#endif //STASE_SEARCH_H
