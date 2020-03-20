CC = gcc -Wall -g

all: main-static time-check-static clean

static:
	make clean
	$(CC) -c library.c
	ar rcs library.a library.o

main-static:
	make static
	$(CC) main.c -o sysVSlib -L. library.a -I .

time-check-static:
	make static
	$(CC) time-check.c -o measureTime -L. library.a -I .

clean:
	rm -f *.o
	rm -f *.a

clean-txt:
	rm -f *.txt