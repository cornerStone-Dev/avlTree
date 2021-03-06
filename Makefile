
all: bin avlTree.o bin/avlTest

avlTree.o: avlTree.c avlTree.h
	gcc -Os -march=native avlTree.c -c -o avlTree.o -Wall -Wextra
	size avlTree.o

bin/avlTest: avlTest.c avlTree.o
	gcc -O2 -march=native avlTest.c -s  -o bin/avlTest avlTree.o -Wall -Wextra

bin:
	mkdir bin

test:
	time ./bin/avlTest

clean:
	rm -f avlTree.o
	rm -f bin/avlTest
