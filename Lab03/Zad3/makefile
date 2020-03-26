CC = gcc -Wall -g

all: main createRand

static:
	make clean-lib
	$(CC) -c library.c
	ar rfc library.a library.o

main:
	make static
	$(CC) main.c -o multiplyMatrices -L. library.a -lm -I .

createRand:
	$(CC) createRandMatrices.c -o randMatrices -L. -lm -I .

clean-lib:
	rm -f *.o
	rm -f *.a

clean-txt:
	rm -f ./matrices/*.txt
	rm -f ./results/*.txt
	rm -f *.txt

clean: clean-lib clean-txt