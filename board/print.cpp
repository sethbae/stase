#include "board.h"

#include <iostream>
using std::cout;

#include <string>
using std::string;


/* print out a human readable chess representation of the board */
void pr_board(const Board & b, string indent) {
    for (int i = 7; i >= 0; --i) {
        cout << indent;
        for (int j = 0; j < 8; ++j) {
            Piece p = b.get(mksq(j, i));
            cout << ptoc(p) << " ";
        }
        cout << "\n";
    }
}

void pr_board(const Board & b) {
    pr_board(b, "");
}

/*void pr_config(const Board & b) {

    cout << "Raw config: ";
    
    int* raw_bin = (int*) &b.conf;
    
    for (int i = 31; i >= 0; --i) {
        if (*raw_bin & (1 << i)) {
            cout << "1";
        } else {
            cout << "0";
        }

        if (i == 0 || i == 1 || i == 5 || i == 9 || i == 15) {
            cout << " ";
        }
    }
    cout << "\n";
    cout << "Turn: " << (b.get_white() ? "White" : "Black") << "\n";

    cout << "Castle Rights: ";
    if (b.get_cas_ws()) cout << "K ";
    if (b.get_cas_wl()) cout << "Q ";
    if (b.get_cas_bs()) cout << "k ";
    if (b.get_cas_bl()) cout << "q ";
    
    if (!(b.get_cas_ws() | b.get_cas_wl() | b.get_cas_bs() | b.get_cas_bl())) {
        cout << " -";
    }

    cout << "\n";

    if (b.get_ep_exists()) {
        cout << "Enpassant: " << sqtos(b.get_ep_sq()) << "\n";
    } else {
        cout << "Enpassant: -\n";
    }

    cout << "Half moves: " << b.get_halfmoves() << "\n";
    cout << "Full moves: " << b.get_wholemoves() << "\n";
}*/

/*void raw(const Board & b) {
    cout << "Raw: ";
    
    int* raw_bin = (int*) &b.conf;
    
    for (int i = 31; i >= 0; --i) {
        if (*raw_bin & (1 << i)) {
            cout << "1";
        } else {
            cout << "0";
        }

        if (i == 0 || i == 1 || i == 5 || i == 9 || i == 16) {
            cout << " ";
        }
    }
    cout << "\n";
}*/


void pr_board_conf(const Board & b, string indent) {
    for (int i = 7; i >= 0; --i) {
        cout << indent;
        for (int j = 0; j < 8; ++j) {
            Piece p = b.get(mksq(j, i));
            cout << ptoc(p) << " ";
        }

        switch (i) {
            case 7: {
                cout << "\tFEN: " << board_to_fen(b);
                break;
            }

            case 6: {
                cout << "\tRaw config: ";
                int* raw_bin = (int*) &b.conf;
                for (int i = 31; i >= 0; --i) {
                    if ( *raw_bin & (1 << i)) {
                        cout << "1";
                    } else {
                        cout << "0";
                    }

                    if (i == 0 || i == 1 || i == 5 || i == 9 || i == 15) {
                        cout << " ";
                    }
                }
                break;
            }

            case 5: {
                cout << "\tTurn: " << (b.get_white() ? "White" : "Black");
                break;
            }

            case 4: {
                cout << "\tCastle Rights: ";
                if (b.get_cas_ws()) cout << "K";
                if (b.get_cas_wl()) cout << "Q";
                if (b.get_cas_bs()) cout << "k";
                if (b.get_cas_bl()) cout << "q";
                if (!(b.get_cas_ws() | b.get_cas_wl() | b.get_cas_bs() | b.get_cas_wl())) {
                    cout << "-";
                }
                
                break;  
            }

            case 3: {
                if (b.get_ep_exists()) {
                    cout << "\tEnpassant: " << sqtos(b.get_ep_sq());
                } else {
                    cout << "\tEnpassant: -";
                }
                break;
            }

            case 2: {
                cout << "\tHalf moves: " << b.get_halfmoves();
                break;
            }

            case 1: {
                cout << "\tFull moves: " << b.get_wholemoves();
                break;
            }
        }

        cout << "\n";
    }
}

/* prints the binary data of a bitmap in a chess board grid */
void pr_bitmap(const Bitmap map) {
    
    uint64_t mask = ( ((uint64_t) 1) << 63);
    for (int i = 0; i < 64; ++i) {
        
        if (map & mask)
            cout << '1';
        else
            cout << '0';
        mask >>= 1;

        if (i % 8 == 7)
            cout << "\n";
    }
}

/* prints out 64 bits of binary data from MSB (left) to LSB (right) */
void pr_bin_64(uint64_t data) {
    
    uint64_t mask = ( ((uint64_t) 1) << 63);
    
    do {
        cout << ((data & mask) ? '1' : '0');
    } while (mask >>= 1);
    
}

