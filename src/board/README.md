## The Board Module

The board directory defines the basics of the game: board, pieces, moves, castling rights and other things which define
fully the state of a game of chess, like move counts and en-passant. 

## Squares

The board is accessed using a _Square_, which packs two coordinates into one. The squares are indexed starting from
the bottom left of the board, so `a1` is square `(0,0)` etc:
```
(0,7)                      (7,7)
    a8 b8 c8 d8 e8 f8 g8 h8
    a7 .  .  .  .  .  .  h7
    a6 .  .  .  .  .  .  h6
    a5 .  .  .  .  .  .  h5
    a4 .  .  .  .  .  .  h4
    a3 .  .  .  .  .  .  h3
    a2 .  .  .  .  .  .  h2
    a1 b1 c1 d1 e1 f1 g1 h1
(0,0)                      (7,0)
```

## Moves, Pieces

The main data types are:
- Square: a single byte which is treated as 2 4-bit numbers - each a coordinate. You can use `mksq(x, y)` to pack one
or `get_x(sq)` to unpack one. `val(sq)` checks that a square represents a valid board location, since 4 bits can encode
8-15 as well.
- Piece: a single byte enum which takes values like W_KNIGHT or B_QUEEN. Corresponds to a wooden piece.
- Ptype: an extension of this, which also includes KNIGHT, BISHOP etc and special values like WHITE, BLACK, EMPTY.
to get the type/colour of a piece, use `type(p)` or `colour(p)`.
- Move: a struct holding a from-square, a to-square, and a 16-bit space for flags, which generally indicate information
about the move (is it a capture, does it give check, is it castling etc).

All of the above have different helper methods accompanying them. Where appropriate there are `stoX` and `Xtos` functions
which translate between strings and the format. Eg `mtos` and `stosq`.

## The board

The board data-type relies on or uses all of the above. It encapsulates:
- 64 squares, each storing a `Ptype` (ie, piece or empty).
- a 32-bit int, which holds bit flags for the following features (exactly as required to play legal chess):
  - Turn colour
  - Castling Rights
  - En-passant exists
  - En-passant file
  - Half move counter
  - Full move counter
- More information about the exact representation is given at the top of `board.cpp`
- Beyond the obvious accessors and mutators, it also has methods to create a new board, on which the given move has
been played. This updates the board position, but also the information about whose go it is etc. There are four methods
in total:
  - Mutate the current board, trustingly
  - Mutate the current board, untrustingly
  - Create and return a successor board, trustingly
  - Create and return a successor board, untrustingly 

  The "trust" determines whether the move can be assumed to have all of the flags set correctly, which may not be true.
When the user enters a move, for example, it won't have any flags set. The untrusting options are denoted with a `_hard`
suffix, and are slower.

## Legal Moves

The remaining functionality implemented in this module is the generation of legal moves. This is a slow process and
should not be done in performance-critical situations. There is a lot of code in `move.cpp` which probably needs a lot
of refactoring to make sense and be anywhere near well designed.

## Bitmaps

There are some partially implemented `Bitmap` functions which are a very good way to compactly store information about
the board. As the use case for these develops, it may be finished/rationalised more so than it currently is.