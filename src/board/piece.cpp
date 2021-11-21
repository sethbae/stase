#include "board.h"

bool is_white(Piece p) { return p & 8; }
bool is_minor_piece(Piece p) {
    switch (type(p)) {
        case KNIGHT:
        case BISHOP:
            return true;
        default:
            return false;
    }
}
bool is_major_piece(Piece p) {
    switch (type(p)) {
        case QUEEN:
        case ROOK:
            return true;
        default:
            return false;
    }
}
bool is_not_pk(Piece p) {
    switch (type(p)) {
        case QUEEN:
        case ROOK:
        case KNIGHT:
        case BISHOP:
            return true;
        default:
            return false;
    }
}
