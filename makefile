FLAGS = -std=c++11 -Wall -Wextra -Werror -Wpedantic
OPT_FLAGS =
DEPS = board.h game.h
OBJ = board/board.o board/move.o board/square.o board/helper.o board/bitmap.o game/gamestate.o game/heur.o

ifneq ($(OPT),none)
	OPT_FLAGS = -O
endif

ifeq ($(OPT),max)
	OPT_FLAGS = -Ofast
endif

%.o: %.cpp $(DEPS)
	g++ -c -o $@ $< $(FLAGS) $(OPT_FLAGS)
	
scratch: $(OBJ) scratch.o
	g++ -o $@ $^ $(FLAGS) $(OPT_FLAGS)

test: $(OBJ) testboard.o
	g++ -o $@ $^ $(FLAGS) $(OPT_FLAGS)
	
bench: $(OBJ) benchboard.o
	g++ -o $@ $^ $(FLAGS) $(OPT_FLAGS)
	
main: $(OBJ)
	
.PHONY: clean

clean:
	rm -f *.o
