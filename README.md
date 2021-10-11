# stase

The STaSe chess engine!
(pronounced stacy)

Currently only implements a heuristic search. There are three basic parts to the strategy:
- Candidate moves:
  - From a given position, suggest possible continuing moves which look good. It's far too slow to
  generate all legal moves, so the current approach is to seek out things like captures, forks and
  checks which are normally useful.
  - The logic is pretty thin in this layer, it shouldn't be trying to do "fork here if that piece won't
  be able to recapture due to the threat of mate", it should be "try this fork", and the resulting
  positions can be judged accordingly, as they result.
  - Perhaps this approach is misguided - the exponential growth of the tree may slow it down too much,
  and it might be better in future to do as much logic as possible here.
- Heuristic evaluation:
  - Given some position, try to judge how much it favours which player, without considering sequences
  of moves, just the structure/material balance etc.
  - There are a number of different things that can be quantified precisely (material balance is an
  obvious one, but something like pawn structure or piece activity too). These are scored, weighted
  and summed to an overall evaluation.
  - The plan is to derive the best weights for each metric according to a long series of games against
  itself. Until that point, they're fairly arbitrary.
- Searching:
  - Treating each board position as a node, expand a tree of possible continuations and select the move
  which is best. This uses the minimax algorithm.
  - In the simplest form, every board is expanded by all of its candidates, and the heuristic evaluation
  is then applied to each leaf node in the resulting tree. The scores of the remaining nodes (specifically
  the root) can then be calculated as per minimax and the best move selected.
  - There are several more complicated ways of doing this though. Complications arise when it would be
  best to revisit nodes, rather than do all of their candidates at once - eg when getting mated, it makes
  sense to circle back and check every possible move (if there's no cleverer way of being sure it's unavoidable).

Each of the three sections has its own (sub)module. The module structure is as follows:
```
/bench
/board
/display
/game
../cands
../heur
/puzzle
/search
/test
/utils
```
More details are given in each module's readme, but roughly speaking:
 - bench:
   - for benchmarking bits of the code. There is some common code, and then a submodule
   named after the module for which it contains benchmarks.
 - board:
   - provides a chess board which stores the board position and config info (eg, whose
   turn it is, and what castling rights they have).
 - display:
   - an ASCII or other interface for playing games against the engine.
 - game:
   - the game logic layer which includes `/cands` and `/heur`. Separate to things which are
   rules of the game, this is the logic of what's a good or bad move, not legal vs illegal.
 - puzzle:
   - helpful stuff for reading the lichess puzzle database
 - search:
   - searches through possible continuations and selects the best move.
 - test:
   - has tests for other modules. Similarly as for benchmarks, there is some common code
   and a submodule with tests for the module it's named after.
 - utils:
   - a few useful bits which are used across different modules.

There are a couple of run configurations which you can try out - mainly benchmarks and tests,
but you can use `scratch.cpp` to play around with different stuff.