compiler = g++ -Wall -Werror -ansi -pedantic

all: rshell

rshell: src/rshell.cpp
	[ ! -d bin ]
	mkdir -p bin;

	$(compiler) src/rshell.cpp -o bin/rshell

clean:	rm bin/*.o bin/rshell.out
