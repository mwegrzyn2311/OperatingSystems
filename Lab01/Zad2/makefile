CC = gcc -Wall -std=gnu99 -ggdb

all: measure-default clean-libs

static:
	make clean-libs
	$(CC) -c library.c
	ar rcs library.a library.o

measure-default:
	make static
	make clean-2
	$(CC) time-check.c -o measure -L. library.a -I .
	./measure

clean:
	rm -f *.o
	rm -f *.so
	rm -f *.a

clean-2:
	rm -f raport2.txt

