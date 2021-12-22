#ifndef STASE_SEARCH_TOOLS_H
#define STASE_SEARCH_TOOLS_H

SearchNode *new_node(const Gamestate & gs, Move m);

void update_score(SearchNode *);
std::vector<SearchNode *> retrieve_best_line(SearchNode *);

void check_abort();

void print_line(std::vector<SearchNode *> & line);
void record_tree_in_file(const std::string & filename, SearchNode * root);

#endif //STASE_SEARCH_TOOLS_H
