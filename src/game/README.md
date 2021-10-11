## The Game Module

This module builds on the `board` module, adding higher-level logic and functionality. It corresponds roughly to answering
the question "is this a good move?" instead of "is this a legal move?"

There are three main parts:
- Gamestates: wrap a board and add more cached information to it. These are used in most places in this module.
- Cands: see the readme in the `/cands` folder
- Heur: see the readme in the `/heur` folder

## Gamestates

Many of the features have not yet been implemented. The idea is to cache information about the position which different
parts of the program will use, or that will be returned to later in the search process, so that it doesn't need to be
recalculated. Examples include:
 - The locations of the kings or all pieces.
 - The control count of each individual square.

The hope is that most of this information can be copied from one gamestate to its successor, thereby saving the need to
recalculate it. This is mostly yet to be implemented.