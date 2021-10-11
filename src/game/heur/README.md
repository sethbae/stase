## Heuristic Evaluation

The aim of this submodule is to apply heuristics to a board position to judge its utility/worth, without calculating
combinations of moves. The 'horizon effect' will hopefully be mitigated before the position reaches this part of the
code (and is not taken account of).

There are several different `Metric`s which are defined, and each produce a score. The scores can then be used in a 
weighted sum to determine the overall score of the position. The hope is that these scores will be reached by using
data and self-play, in some fashion, and until then they are fairly arbitrary.

Some example metrics are:
- Gamma activity: the 3rd of three different definitions of control (as explained in `game/piece.cpp`).
- Centre control: who controls the central 4 squares better?
- Isolanis: does one player have more isolated pawns?

In future, the metrics applied to a position could be dynamically selected (eg, on phase of the game), and their
weights could be dynamically adjusted. For now, every metric is always applied.
