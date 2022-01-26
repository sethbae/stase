#ifndef STASE_GAME_H
#define STASE_GAME_H

#include <string>
#include "board.h"
#include "../../src/game/eval.hpp"

class Gamestate;

enum GamePhase : uint8_t {
    OPENING = 0,
    MIDGAME = 1,
    ENDGAME = 2
};
inline std::string name(GamePhase phase) {
    switch (phase) {
        case OPENING: return "OPENING";
        case MIDGAME: return "MIDGAME";
        case ENDGAME: return "ENDGAME";
        default: return "INVALID";
    }
}

enum CandList {
    CRITICAL,
    MEDIAL,
    FINAL,
    LEGAL
};
inline std::string name(CandList cand_list) {
    switch (cand_list) {
        case CRITICAL: return "CRITICAL";
        case MEDIAL: return "MEDIAL";
        case FINAL: return "FINAL";
        case LEGAL: return "LEGAL";
        default: return "INVALID";
    }
}

struct CandSet {
    std::vector<Move> critical;
    std::vector<Move> medial;
    std::vector<Move> final;
    std::vector<Move> legal;

    CandSet() :
            critical(std::vector<Move>()),
            medial(std::vector<Move>()),
            final(std::vector<Move>()),
            legal(std::vector<Move>())
    {}

    inline bool empty() { return critical.empty() && medial.empty() && final.empty() && legal.empty(); }
    inline int size() {
        if (legal.size()) {
            return legal.size();
        }
        return critical.size() + medial.size() + final.size();
    }
    inline std::vector<Move> get_list(CandList cand_list) {
        switch (cand_list) {
            case CRITICAL: return critical;
            case MEDIAL: return medial;
            case FINAL: return final;
            case LEGAL: return legal;
        }
        // impossible
        return {};
    }
    inline void clear_list(CandList cand_list) {
        switch (cand_list) {
            case CRITICAL: critical.clear(); break;
            case MEDIAL: medial.clear(); break;
            case FINAL: final.clear(); break;
            case LEGAL: legal.clear(); break;
        }
    }
};

Gamestate * fresh_gamestate(const std::string &);
Gamestate * next_gamestate(const Gamestate *, const Move);

CandSet * cands(Gamestate &);
CandSet * cands(Gamestate &, CandSet *);
Eval heur(const Gamestate &);
float quiess(const Gamestate &);

void print_cand_set(const Gamestate &, const CandSet &, std::ostream &);
Eval heur_with_description(const Gamestate &);
CandSet * cands_report(Gamestate &);

#endif //STASE_GAME_H
