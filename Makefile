opt = "-Wall -Werror -ansi -pedantic"

all: if[ ! -d "./bin" ];\
     then mkdir bin;\
     fi
     g++ $(opt) src/rshell.cpp -o bin/rshell

rshell: if[ ! -d ".\bin" ];\
	then mkdir bin;\
	fi
	g++ $(opt) src/rshell.cpp -o bin/rshell

clean:	rm *.o rshell.out
