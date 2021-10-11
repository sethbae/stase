## Candidate moves

### Hooks and Responders

Generating candidate moves is done in a somewhat complex way. There are two key types:
- Hooks: these act like predicates, mapping `(Board, Square) -> {True, False}`
  - For example, the `development` hook answers the question "does this square contain an undeveloped piece?"
- Responders: these are given a square on which a Hook has returned true, along with optionally some more information,
and generate moves on that basis. 
  - For example, the `development` Responder would suggest some squares that the piece
  could move to.

### Feature Frames

These communicate through the medium of so called `FeatureFrames`: a Hook writes a feature frame, and a responder 
generates moves on the basis of one. A `FeatureFrame` might specify only a single square, on which it the feature occurs,
or it might also provide a secondary square or some config integers (`conf_1` and `conf_2`).

This abstraction is useful for several reasons. It allows the decoupling of specific hooks from specific responders, and
it will hopefully allow the copying of feature frames between positions, for pieces which have not been affected by the
move. A Hook will therefore only have to run on a few squares which need updating, and not the entire board.

### Feature Handlers

The final abstraction is that of the `FeatureHandler`. This encapsulates a piece of chess-playing logic: if my piece is
threatened, I should move it, or capture more valuable pieces, or defend it. In order to do this, it comprises:
- A single hook
- An ordered list of _friendly_ responders: things you should do if its your piece (eg that's attacked).
- An ordered list of _enemy_ responders: things you should do if its your opponent's piece (eg that's attacked).

The way that moves suggested by handlers are combined and/or capped, and the overall limits on the number of moves that
the `cands` function will return, are subject to change (and need a proper design!). I hope that in future the process
will be re-entrant, in the sense that search can ask it for 5 moves, come back later and ask for another 5 etc, but this
is yet to be implemented.