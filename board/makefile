FLAGS: -std=c++11 -Wall -Wextra -Werror
OPT: -O
SUPEROPT: -Ofast

DEPS = board.h
OBJ = board.o move.o square.o helper.o bitmap.o gamestate.o

%.o: %.cpp $(DEPS)
	g++ -c -o $@ $< $(FLAGS)
	
scratch: $(OBJ) scratch.o
	g++ -o $@ $^ $(FLAGS)

test: $(OBJ) testboard.o
	g++ -o $@ $^ $(FLAGS)
	
bench: $(OBJ) benchboard.o
	g++ -o $@ $^ $(FLAGS)

bench_opt: $(OBJ) benchboard.o
	g++ -o $@ $^ $(FLAGS) $(OPT)
	
.PHONY: clean

clean:
	rm -f *.o
