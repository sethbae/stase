#include "board.h"
#include "board.hpp"

GameStatus game_rules::check_status(const Board & b) {

    bool in_check = game_rules::in_check(b);
    std::vector<Move> legal_moves = game_rules::legal_moves(b);

    if (legal_moves.empty()) {
        if (!in_check) {
            return DRAW_BY_STALEMATE;
        }
        return b.get_white() ? BLACK_WON : WHITE_WON;
    }

    if (b.get_halfmoves() >= 50) {
        return DRAW_BY_FIFTY_MOVES;
    }

    return ONGOING;
}

GameStatus game_rules::check_status(const std::string & fen) {
    return game_rules::check_status(board_utils::fen_to_board(fen));
}

GameStatus game_rules::check_status(const std::vector<Board> & boards) {

    const Board & current_pos =boards.back();
    GameStatus status = game_rules::check_status(current_pos);

    if (status != ONGOING) {
        return status;
    }

    int count = 0;

    // we now search the board history
    for (int i = (int)boards.size() - 1; i >= 0; --i) {
        const Board & b = boards[i];
        if (b.equivalent(current_pos)) {
            if (++count == 3) {
                return DRAW_BY_THREEFOLD;
            }
        }
    }
    return ONGOING;
}

GameStatus game_rules::check_status(const std::vector<std::string> & fens) {
    std::vector<Board> boards(fens.size());
    for (const std::string & fen : fens) {
        boards.push_back(board_utils::fen_to_board(fen));
    }
    return game_rules::check_status(boards);
}
