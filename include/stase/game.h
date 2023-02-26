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

    inline bool empty() const { return critical.empty() && medial.empty() && final.empty() && legal.empty(); }
    inline int size() const {
        if (!legal.empty()) {
            return (int) legal.size();
        }
        return (int) (critical.size() + medial.size() + final.size());
    }

    inline std::vector<Move> & get_list(CandList cand_list) {
        switch (cand_list) {
            case CRITICAL: return critical;
            case MEDIAL: return medial;
            case FINAL: return final;
            case LEGAL: return legal;
            default: return critical;
        }
    }

    inline void clear_list(CandList cand_list) {
        switch (cand_list) {
            case CRITICAL: critical.clear(); break;
            case MEDIAL: medial.clear(); break;
            case FINAL: final.clear(); break;
            case LEGAL: legal.clear(); break;
        }
    }

    /**
     * Sorts the specified list by order of decreasing score.
     */
    inline void order_list(CandList cand_list) {
        std::vector<Move> & list = get_list(cand_list);
        for (int i = 0; i < list.size(); ++i) {
            for (int j = 0; j < i; ++j) {
                if (list[i].get_score() > list[j].get_score()) {
                    Move temp = list[j];
                    list[j] = list[i];
                    list[i] = temp;
                }
            }
        }
    }
};

class MetricWeights {
public:
    const int piece_activity_alpha_opening;
    const int piece_activity_alpha_midgame;
    const int piece_activity_alpha_endgame;
    const int piece_activity_beta_opening;
    const int piece_activity_beta_midgame;
    const int piece_activity_beta_endgame;
    const int piece_activity_gamma_opening;
    const int piece_activity_gamma_midgame;
    const int piece_activity_gamma_endgame;
    const int open_line_control_opening;
    const int open_line_control_midgame;
    const int open_line_control_endgame;
    const int centre_control_opening;
    const int centre_control_midgame;
    const int centre_control_endgame;
    const int defended_pawns_opening;
    const int defended_pawns_midgame;
    const int defended_pawns_endgame;
    const int isolated_pawns_opening;
    const int isolated_pawns_midgame;
    const int isolated_pawns_endgame;
    const int central_pawns_opening;
    const int central_pawns_midgame;
    const int central_pawns_endgame;
    const int far_advanced_pawn_opening;
    const int far_advanced_pawns_midgame;
    const int far_advanced_pawns_endgame;
    const int development_opening;
    const int development_midgame;
    const int development_endgame;
    const int castling_opening;
    const int castling_midgame;
    const int castling_endgame;
    const int castling_rights_opening;
    const int castling_rights_midgame;
    const int castling_rights_endgame;
    const int pawns_defend_king_opening;
    const int pawns_defend_king_midgame;
    const int pawns_defend_king_endgame;
    const int control_near_king_opening;
    const int control_near_king_midgame;
    const int control_near_king_endgame;
    const int king_exposure_opening;
    const int king_exposure_midgame;
    const int king_exposure_endgame;
    const int unconventional_king_opening;
    const int unconventional_king_midgame;
    const int unconventional_king_endgame;
    const int promotion_squares_opening;
    const int promotion_squares_midgame;
    const int promotion_squares_endgame;
    const int mating_net_opening;
    const int mating_net_midgame;
    const int mating_net_endgame;

    MetricWeights(
            int piece_activity_alpha_opening,
            int piece_activity_alpha_midgame,
            int piece_activity_alpha_endgame,
            int piece_activity_beta_opening,
            int piece_activity_beta_midgame,
            int piece_activity_beta_endgame,
            int piece_activity_gamma_opening,
            int piece_activity_gamma_midgame,
            int piece_activity_gamma_endgame,
            int open_line_control_opening,
            int open_line_control_midgame,
            int open_line_control_endgame,
            int centre_control_opening,
            int centre_control_midgame,
            int centre_control_endgame,
            int defended_pawns_opening,
            int defended_pawns_midgame,
            int defended_pawns_endgame,
            int isolated_pawns_opening,
            int isolated_pawns_midgame,
            int isolated_pawns_endgame,
            int central_pawns_opening,
            int central_pawns_midgame,
            int central_pawns_endgame,
            int far_advanced_pawn_opening,
            int far_advanced_pawns_midgame,
            int far_advanced_pawns_endgame,
            int development_opening,
            int development_midgame,
            int development_endgame,
            int castling_opening,
            int castling_midgame,
            int castling_endgame,
            int castling_rights_opening,
            int castling_rights_midgame,
            int castling_rights_endgame,
            int pawns_defend_king_opening,
            int pawns_defend_king_midgame,
            int pawns_defend_king_endgame,
            int control_near_king_opening,
            int control_near_king_midgame,
            int control_near_king_endgame,
            int king_exposure_opening,
            int king_exposure_midgame,
            int king_exposure_endgame,
            int unconventional_king_opening,
            int unconventional_king_midgame,
            int unconventional_king_endgame,
            int promotion_squares_opening,
            int promotion_squares_midgame,
            int promotion_squares_endgame,
            int mating_net_opening,
            int mating_net_midgame,
            int mating_net_endgame)
            :
            piece_activity_alpha_opening(piece_activity_alpha_opening),
            piece_activity_alpha_midgame(piece_activity_alpha_midgame),
            piece_activity_alpha_endgame(piece_activity_alpha_endgame),
            piece_activity_beta_opening(piece_activity_beta_opening),
            piece_activity_beta_midgame(piece_activity_beta_midgame),
            piece_activity_beta_endgame(piece_activity_beta_endgame),
            piece_activity_gamma_opening(piece_activity_gamma_opening),
            piece_activity_gamma_midgame(piece_activity_gamma_midgame),
            piece_activity_gamma_endgame(piece_activity_gamma_endgame),
            open_line_control_opening(open_line_control_opening),
            open_line_control_midgame(open_line_control_midgame),
            open_line_control_endgame(open_line_control_endgame),
            centre_control_opening(centre_control_opening),
            centre_control_midgame(centre_control_midgame),
            centre_control_endgame(centre_control_endgame),
            defended_pawns_opening(defended_pawns_opening),
            defended_pawns_midgame(defended_pawns_midgame),
            defended_pawns_endgame(defended_pawns_endgame),
            isolated_pawns_opening(isolated_pawns_opening),
            isolated_pawns_midgame(isolated_pawns_midgame),
            isolated_pawns_endgame(isolated_pawns_endgame),
            central_pawns_opening(central_pawns_opening),
            central_pawns_midgame(central_pawns_midgame),
            central_pawns_endgame(central_pawns_endgame),
            far_advanced_pawn_opening(far_advanced_pawn_opening),
            far_advanced_pawns_midgame(far_advanced_pawns_midgame),
            far_advanced_pawns_endgame(far_advanced_pawns_endgame),
            development_opening(development_opening),
            development_midgame(development_midgame),
            development_endgame(development_endgame),
            castling_opening(castling_opening),
            castling_midgame(castling_midgame),
            castling_endgame(castling_endgame),
            castling_rights_opening(castling_rights_opening),
            castling_rights_midgame(castling_rights_midgame),
            castling_rights_endgame(castling_rights_endgame),
            pawns_defend_king_opening(pawns_defend_king_opening),
            pawns_defend_king_midgame(pawns_defend_king_midgame),
            pawns_defend_king_endgame(pawns_defend_king_endgame),
            control_near_king_opening(control_near_king_opening),
            control_near_king_midgame(control_near_king_midgame),
            control_near_king_endgame(control_near_king_endgame),
            king_exposure_opening(king_exposure_opening),
            king_exposure_midgame(king_exposure_midgame),
            king_exposure_endgame(king_exposure_endgame),
            unconventional_king_opening(unconventional_king_opening),
            unconventional_king_midgame(unconventional_king_midgame),
            unconventional_king_endgame(unconventional_king_endgame),
            promotion_squares_opening(promotion_squares_opening),
            promotion_squares_midgame(promotion_squares_midgame),
            promotion_squares_endgame(promotion_squares_endgame),
            mating_net_opening(mating_net_opening),
            mating_net_midgame(mating_net_midgame),
            mating_net_endgame(mating_net_endgame)
    {}

    inline int weight(int id, GamePhase phase) const {
        switch (id) {
            case 0:
                switch (phase) {
                    case OPENING: return piece_activity_alpha_opening;
                    case MIDGAME: return piece_activity_alpha_midgame;
                    case ENDGAME: return piece_activity_alpha_endgame;
                }
            case 1:
                switch (phase) {
                    case OPENING: return piece_activity_beta_opening;
                    case MIDGAME: return piece_activity_beta_midgame;
                    case ENDGAME: return piece_activity_beta_endgame;
                }
            case 2:
                switch (phase) {
                    case OPENING: return piece_activity_gamma_opening;
                    case MIDGAME: return piece_activity_gamma_midgame;
                    case ENDGAME: return piece_activity_gamma_endgame;
                }
            case 3:
                switch (phase) {
                    case OPENING: return open_line_control_opening;
                    case MIDGAME: return open_line_control_midgame;
                    case ENDGAME: return open_line_control_endgame;
                }
            case 4:
                switch (phase) {
                    case OPENING: return centre_control_opening;
                    case MIDGAME: return centre_control_midgame;
                    case ENDGAME: return centre_control_endgame;
                }
            case 5:
                switch (phase) {
                    case OPENING: return defended_pawns_opening;
                    case MIDGAME: return defended_pawns_midgame;
                    case ENDGAME: return defended_pawns_endgame;
                }
            case 6:
                switch (phase) {
                    case OPENING: return isolated_pawns_opening;
                    case MIDGAME: return isolated_pawns_midgame;
                    case ENDGAME: return isolated_pawns_endgame;
                }
            case 7:
                switch (phase) {
                    case OPENING: return central_pawns_opening;
                    case MIDGAME: return central_pawns_midgame;
                    case ENDGAME: return central_pawns_endgame;
                }
            case 8:
                switch (phase) {
                    case OPENING: return far_advanced_pawn_opening;
                    case MIDGAME: return far_advanced_pawns_midgame;
                    case ENDGAME: return far_advanced_pawns_endgame;
                }
            case 9:
                switch (phase) {
                    case OPENING: return development_opening;
                    case MIDGAME: return development_midgame;
                    case ENDGAME: return development_endgame;
                }
            case 10:
                switch (phase) {
                    case OPENING: return castling_opening;
                    case MIDGAME: return castling_midgame;
                    case ENDGAME: return castling_endgame;
                }
            case 11:
                switch (phase) {
                    case OPENING: return castling_rights_opening;
                    case MIDGAME: return castling_rights_midgame;
                    case ENDGAME: return castling_rights_endgame;
                }
            case 12:
                switch (phase) {
                    case OPENING: return pawns_defend_king_opening;
                    case MIDGAME: return pawns_defend_king_midgame;
                    case ENDGAME: return pawns_defend_king_endgame;
                }
            case 13:
                switch (phase) {
                    case OPENING: return control_near_king_opening;
                    case MIDGAME: return control_near_king_midgame;
                    case ENDGAME: return control_near_king_endgame;
                }
            case 14:
                switch (phase) {
                    case OPENING: return king_exposure_opening;
                    case MIDGAME: return king_exposure_midgame;
                    case ENDGAME: return king_exposure_endgame;
                }
            case 15:
                switch (phase) {
                    case OPENING: return unconventional_king_opening;
                    case MIDGAME: return unconventional_king_midgame;
                    case ENDGAME: return unconventional_king_endgame;
                }
            case 16:
                switch (phase) {
                    case OPENING: return promotion_squares_opening;
                    case MIDGAME: return promotion_squares_midgame;
                    case ENDGAME: return promotion_squares_endgame;
                }
            case 17:
                switch (phase) {
                    case OPENING: return mating_net_opening;
                    case MIDGAME: return mating_net_midgame;
                    case ENDGAME: return mating_net_endgame;
                }
        }
        return 0;
    }
};

const MetricWeights DEFAULT_METRIC_WEIGHTS(
        // alpha activity
        600, 600, 600,
        // beta activity
        600, 600, 600,
        // gamma activity
        600, 600, 600,
        // open line control
        800, 1000, 800,
        // centre control
        2000, 1500, 750,
        // defended pawns
        400, 500, 750,
        // isolated pawns
        300, 400, 600,
        // central pawns
        600, 500, 400,
        // far advanced pawns
        100, 400, 1000,
        // development
        1000, 500, 0,
        // castling
        300, 200, 0,
        // castling rights
        400, 300, 0,
        // pawns defend king
        500, 1000, 400,
        // control near king
        500, 1000, 400,
        // king exposure
        500, 1000, 400,
        // unconventional king
        1000, 500, 0,
        // promotion squares
        0, 1000, 2000,
        // mating net
        2000, 2000, 10000
);

CandSet * cands(Gamestate &, CandSet *);
Eval heur(const Gamestate &, const MetricWeights * = &DEFAULT_METRIC_WEIGHTS);
float quiess(Gamestate &);

void print_cand_set(const Gamestate &, const CandSet &, std::ostream &);
Eval heur_with_description(const Gamestate &, const MetricWeights * = &DEFAULT_METRIC_WEIGHTS);
CandSet * cands_report(Gamestate &);

#endif //STASE_GAME_H
