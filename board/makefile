FLAGS: -std=c++11 -Wall -Wextra -Werror -Wpedantic

DEPS = board.h
OBJ = board.o move.o square.o helper.o bitmap.o gamestate.o

ifneq ($(OPT),none)
	FLAGS += -O
endif

ifeq ($(OPT),max)
	FLAGS += -Ofast
endif

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
	
main: $(OBJ)
	
.PHONY: clean

clean:
	rm -f *.o
