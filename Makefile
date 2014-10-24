opt = -Wall -Werror -ansi -pedantic

all: rshell

rshell: mkdir -p bin
	g++ $(opt) src/rshell.cpp -o bin/rshell

clean:	rm *.o rshell.out
