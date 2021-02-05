# stase

The STaSe chess engine!
(pronounced stacy)

Below is a rough separation of different program components which could be separated in different files and implemented separately.
We could even define precise interfaces between them.

#####################################

Display

Human Interface (different boards/interaction controls/timing)

#####################################

Game logic 
    - how long do I spend thinking here? what do I do when it isn't my move?
Threads 
    - how many threads are needed/what shall each one do?

#####################################

Search
    - from a given position, what is the best line of play?
    - when should I get more candidates to look at?
    - pruning

#####################################

Candidates
    - what moves shall we look at from a given position?
    - rough approach to which moves should be investigated first

Heuristic Evaluation (Leaf)
    - does this end position look like a good one?

#####################################

Helper functions:
    make_move (board, move)
    legal_moves (board)
    fentoboard(fen)
    boardtofen(board)

Datatypes:
    Node
    Board position
    Move
