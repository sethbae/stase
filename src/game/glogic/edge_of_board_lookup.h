#ifndef STASE_EDGE_OF_BOARD_LOOKUP_H
#define STASE_EDGE_OF_BOARD_LOOKUP_H

inline Square edge_of_board(const Square s, const int delta_ordinal) {
    const static Square lookup[8][8][8]{
        {                 // ---->
            {stosq("h8"), stosq("h8"), stosq("h8"), stosq("h8"), stosq("h8"), stosq("h8"), stosq("h8"), stosq("h8") },
            {stosq("h7"), stosq("h7"), stosq("h7"), stosq("h7"), stosq("h7"), stosq("h7"), stosq("h7"), stosq("h7") },
            {stosq("h6"), stosq("h6"), stosq("h6"), stosq("h6"), stosq("h6"), stosq("h6"), stosq("h6"), stosq("h6") },
            {stosq("h5"), stosq("h5"), stosq("h5"), stosq("h5"), stosq("h5"), stosq("h5"), stosq("h5"), stosq("h5") },
            {stosq("h4"), stosq("h4"), stosq("h4"), stosq("h4"), stosq("h4"), stosq("h4"), stosq("h4"), stosq("h4") },
            {stosq("h3"), stosq("h3"), stosq("h3"), stosq("h3"), stosq("h3"), stosq("h3"), stosq("h3"), stosq("h3") },
            {stosq("h2"), stosq("h2"), stosq("h2"), stosq("h2"), stosq("h2"), stosq("h2"), stosq("h2"), stosq("h2") },
            {stosq("h1"), stosq("h1"), stosq("h1"), stosq("h1"), stosq("h1"), stosq("h1"), stosq("h1"), stosq("h1") },
        },
        {                 // straight up
            {stosq("a8"), stosq("b8"), stosq("c8"), stosq("d8"), stosq("e8"), stosq("f8"), stosq("g8"), stosq("h8") },
            {stosq("a8"), stosq("b8"), stosq("c8"), stosq("d8"), stosq("e8"), stosq("f8"), stosq("g8"), stosq("h8") },
            {stosq("a8"), stosq("b8"), stosq("c8"), stosq("d8"), stosq("e8"), stosq("f8"), stosq("g8"), stosq("h8") },
            {stosq("a8"), stosq("b8"), stosq("c8"), stosq("d8"), stosq("e8"), stosq("f8"), stosq("g8"), stosq("h8") },
            {stosq("a8"), stosq("b8"), stosq("c8"), stosq("d8"), stosq("e8"), stosq("f8"), stosq("g8"), stosq("h8") },
            {stosq("a8"), stosq("b8"), stosq("c8"), stosq("d8"), stosq("e8"), stosq("f8"), stosq("g8"), stosq("h8") },
            {stosq("a8"), stosq("b8"), stosq("c8"), stosq("d8"), stosq("e8"), stosq("f8"), stosq("g8"), stosq("h8") },
            {stosq("a8"), stosq("b8"), stosq("c8"), stosq("d8"), stosq("e8"), stosq("f8"), stosq("g8"), stosq("h8") },
        },
        {                  // up right
            {stosq("a8"), stosq("b8"), stosq("c8"), stosq("d8"), stosq("e8"), stosq("f8"), stosq("g8"), stosq("h8") },
            {stosq("b8"), stosq("c8"), stosq("d8"), stosq("e8"), stosq("f8"), stosq("g8"), stosq("h8"), stosq("h7") },
            {stosq("c8"), stosq("d8"), stosq("e8"), stosq("f8"), stosq("g8"), stosq("h8"), stosq("h7"), stosq("h6") },
            {stosq("d8"), stosq("e8"), stosq("f8"), stosq("g8"), stosq("h8"), stosq("h7"), stosq("h6"), stosq("h5") },
            {stosq("e8"), stosq("f8"), stosq("g8"), stosq("h8"), stosq("h7"), stosq("h6"), stosq("h5"), stosq("h4") },
            {stosq("f8"), stosq("g8"), stosq("h8"), stosq("h7"), stosq("h6"), stosq("h5"), stosq("h4"), stosq("h3") },
            {stosq("g8"), stosq("h8"), stosq("h7"), stosq("h6"), stosq("h5"), stosq("h4"), stosq("h3"), stosq("h2") },
            {stosq("h8"), stosq("h7"), stosq("h6"), stosq("h5"), stosq("h4"), stosq("h3"), stosq("h2"), stosq("h1") },
        },
        {                   // down right
            {stosq("h1"), stosq("h2"), stosq("h3"), stosq("h4"), stosq("h5"), stosq("h6"), stosq("h7"), stosq("h8") },
            {stosq("g1"), stosq("h1"), stosq("h2"), stosq("h3"), stosq("h4"), stosq("h5"), stosq("h6"), stosq("h7") },
            {stosq("f1"), stosq("g1"), stosq("h1"), stosq("h2"), stosq("h3"), stosq("h4"), stosq("h5"), stosq("h6") },
            {stosq("e1"), stosq("f1"), stosq("g1"), stosq("h1"), stosq("h2"), stosq("h3"), stosq("h4"), stosq("h5") },
            {stosq("d1"), stosq("e1"), stosq("f1"), stosq("g1"), stosq("h1"), stosq("h2"), stosq("h3"), stosq("h4") },
            {stosq("c1"), stosq("d1"), stosq("e1"), stosq("f1"), stosq("g1"), stosq("h1"), stosq("h2"), stosq("h3") },
            {stosq("b1"), stosq("c1"), stosq("d1"), stosq("e1"), stosq("f1"), stosq("g1"), stosq("h1"), stosq("h2") },
            {stosq("a1"), stosq("b1"), stosq("c1"), stosq("d1"), stosq("e1"), stosq("f1"), stosq("g1"), stosq("h1") },
        },
        {                   //  <----
            {stosq("a8"), stosq("a8"), stosq("a8"), stosq("a8"), stosq("a8"), stosq("a8"), stosq("a8"), stosq("a8") },
            {stosq("a7"), stosq("a7"), stosq("a7"), stosq("a7"), stosq("a7"), stosq("a7"), stosq("a7"), stosq("a7") },
            {stosq("a6"), stosq("a6"), stosq("a6"), stosq("a6"), stosq("a6"), stosq("a6"), stosq("a6"), stosq("a6") },
            {stosq("a5"), stosq("a5"), stosq("a5"), stosq("a5"), stosq("a5"), stosq("a5"), stosq("a5"), stosq("a5") },
            {stosq("a4"), stosq("a4"), stosq("a4"), stosq("a4"), stosq("a4"), stosq("a4"), stosq("a4"), stosq("a4") },
            {stosq("a3"), stosq("a3"), stosq("a3"), stosq("a3"), stosq("a3"), stosq("a3"), stosq("a3"), stosq("a3") },
            {stosq("a2"), stosq("a2"), stosq("a2"), stosq("a2"), stosq("a2"), stosq("a2"), stosq("a2"), stosq("a2") },
            {stosq("a1"), stosq("a1"), stosq("a1"), stosq("a1"), stosq("a1"), stosq("h1"), stosq("h1"), stosq("h1") },
        },
        {                   // straight down
            {stosq("a1"), stosq("b1"), stosq("c1"), stosq("d1"), stosq("e1"), stosq("f1"), stosq("g1"), stosq("h1") },
            {stosq("a1"), stosq("b1"), stosq("c1"), stosq("d1"), stosq("e1"), stosq("f1"), stosq("g1"), stosq("h1") },
            {stosq("a1"), stosq("b1"), stosq("c1"), stosq("d1"), stosq("e1"), stosq("f1"), stosq("g1"), stosq("h1") },
            {stosq("a1"), stosq("b1"), stosq("c1"), stosq("d1"), stosq("e1"), stosq("f1"), stosq("g1"), stosq("h1") },
            {stosq("a1"), stosq("b1"), stosq("c1"), stosq("d1"), stosq("e1"), stosq("f1"), stosq("g1"), stosq("h1") },
            {stosq("a1"), stosq("b1"), stosq("c1"), stosq("d1"), stosq("e1"), stosq("f1"), stosq("g1"), stosq("h1") },
            {stosq("a1"), stosq("b1"), stosq("c1"), stosq("d1"), stosq("e1"), stosq("f1"), stosq("g1"), stosq("h1") },
            {stosq("a1"), stosq("b1"), stosq("c1"), stosq("d1"), stosq("e1"), stosq("f1"), stosq("g1"), stosq("h1") },
        },
        {                   // down left
            {stosq("a8"), stosq("a7"), stosq("a6"), stosq("a5"), stosq("a4"), stosq("a3"), stosq("a2"), stosq("a1") },
            {stosq("a7"), stosq("a6"), stosq("a5"), stosq("a4"), stosq("a3"), stosq("a2"), stosq("a1"), stosq("b1") },
            {stosq("a6"), stosq("a5"), stosq("a4"), stosq("a3"), stosq("a2"), stosq("a1"), stosq("b1"), stosq("c1") },
            {stosq("a5"), stosq("a4"), stosq("a3"), stosq("a2"), stosq("a1"), stosq("b1"), stosq("c1"), stosq("d1") },
            {stosq("a4"), stosq("a3"), stosq("a2"), stosq("a1"), stosq("b1"), stosq("c1"), stosq("d1"), stosq("e1") },
            {stosq("a3"), stosq("a2"), stosq("a1"), stosq("b1"), stosq("c1"), stosq("d1"), stosq("e1"), stosq("f1") },
            {stosq("a2"), stosq("a1"), stosq("b1"), stosq("c1"), stosq("d1"), stosq("e1"), stosq("f1"), stosq("g1") },
            {stosq("a1"), stosq("b1"), stosq("c1"), stosq("d1"), stosq("e1"), stosq("f1"), stosq("g1"), stosq("h1") },
        },
        {                   // up left
            {stosq("a8"), stosq("b8"), stosq("c8"), stosq("d8"), stosq("e8"), stosq("f8"), stosq("g8"), stosq("h8") },
            {stosq("a7"), stosq("a8"), stosq("b8"), stosq("c8"), stosq("d8"), stosq("e8"), stosq("f8"), stosq("g8") },
            {stosq("a6"), stosq("a7"), stosq("a8"), stosq("b8"), stosq("c8"), stosq("d8"), stosq("e8"), stosq("f8") },
            {stosq("a5"), stosq("a6"), stosq("a7"), stosq("a8"), stosq("b8"), stosq("c8"), stosq("d8"), stosq("e8") },
            {stosq("a4"), stosq("a5"), stosq("a6"), stosq("a7"), stosq("a8"), stosq("b8"), stosq("c8"), stosq("d8") },
            {stosq("a3"), stosq("a4"), stosq("a5"), stosq("a6"), stosq("a7"), stosq("a8"), stosq("b8"), stosq("c8") },
            {stosq("a2"), stosq("a3"), stosq("a4"), stosq("a5"), stosq("a6"), stosq("a7"), stosq("a8"), stosq("b8") },
            {stosq("a1"), stosq("a2"), stosq("a3"), stosq("a4"), stosq("a5"), stosq("a6"), stosq("a7"), stosq("a8") },
        },
    };
    return lookup[delta_ordinal][7 - s.y][s.x];
}

#endif //STASE_EDGE_OF_BOARD_LOOKUP_H
