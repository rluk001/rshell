opt = "-W -Wall -Werror -ansi -pedantic"

all: rshell

rshell: g++ $(opt) src/rshell.cpp -o src/rshell.out

clean:	rm *.o rshell.out
