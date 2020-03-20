CC = gcc -Wall -std=gnu99 -ggdb

all: main-static main-shared clean

static:
	make clean
	$(CC) -c library.c
	ar rcs library.a library.o

shared:
	make clean-libs
	$(CC) -c -fPIC library.c
	$(CC) -shared -fPIC -o library.so library.o

main-static:
	make static
	$(CC) main.c -o blocks-stuff -L. library.a -I .
	
main-shared:
	make shared
	$(CC) main.c -o blocks-stuff -L. library.so -Wl,-rpath=. -I .

clean:
	rm -f *.o
	rm -f *.so
	rm -f *.a
