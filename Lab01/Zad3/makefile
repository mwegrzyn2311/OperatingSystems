CC = gcc -Wall -std=gnu99 -ggdb

all: main-static main-shared main-dynamic measure-static measure-shared measure-dynamic create-raport3a create-raport3b clean-libs

static:
	make clean-libs
	$(CC) -c library.c
	ar rcs library.a library.o

shared:
	make clean-libs
	$(CC) -c -fPIC library.c
	$(CC) -shared -fPIC -o library.so library.o

dynamic:
	make clean-libs
	$(CC) -fPIC -c library.c -o library.o
	$(CC) -Wl,-rpath=. -fPIC -shared -o library.so library.o

main-static:
	make static
	$(CC) main.c -o blocks-stuff -L. library.a -I .
	
main-shared:
	make shared
	$(CC) main.c -o blocks-stuff -L. library.so -Wl,-rpath=. -I .

main-dynamic:
	make dynamic
	$(CC) main.c -o block-stuff -fPIC -L. -ldl -D DYNAMIC

measure-default:
	make static
	make clean-2
	$(CC) time-check.c -o measure -L. library.a -I .
	./measure

create-raport3a: clean-3a measure-static measure-shared

measure-static:
	make static
	$(CC) time-check.c -o measure -L. library.a -I .
	echo "Time measured using static-lib and default flags:" >> raport3a.txt
	./measure raport3a.txt

measure-shared:
	make shared
	$(CC) time-check.c -o measure -L. library.so -Wl,-rpath=. -I .
	echo "Time measured using shared-lib and default flags:" >> raport3a.txt
	./measure raport3a.txt

measure-dynamic:
	make dynamic
	$(CC) time-check.c -o measure -ldl -D DYNAMIC
	echo "Time measured using dynamic-lib and default flags:" >> raport3a.txt
	./measure raport3a.txt


static-O3:
	make clean-libs
	$(CC) -c -O3 library.c
	ar rcs library.a library.o

shared-O3:
	make clean-libs
	$(CC) -c -O3 -fPIC library.c
	$(CC) -shared -fPIC -o library.so library.o

dynamic-O3:
	make clean-libs
	$(CC) -fPIC -c -O3 library.c -o library.o
	$(CC) -Wl,-rpath=. -fPIC -shared -o library.so library.o

measure-static-O3:
	make static-O3
	$(CC) -O3 time-check.c -o measure -L. library.a -I .
	echo "Time measured using static-lib and -O3 flag:" >> raport3a.txt
	./measure raport3b.txt

measure-shared-O3:
	make shared-O3
	$(CC) -O3 time-check.c -o measure -L. library.so -Wl,-rpath=. -I .
	echo "Time measured using shared-lib and default -O3 flag:" >> raport3a.txt
	./measure raport3b.txt

measure-dynamic-O3:
	make dynamic-O3
	$(CC) -O3 time-check.c -o measure -ldl -D DYNAMIC
	echo "Time measured using dynamic-lib and default -O3 flag:" >> raport3a.txt
	./measure raport3b.txt


static-Os:
	make clean-libs
	$(CC) -c -Os library.c
	ar rcs library.a library.o

shared-Os:
	make clean-libs
	$(CC) -c -Os -fPIC library.c
	$(CC) -shared -fPIC -o library.so library.o

dynamic-Os:
	make clean-libs
	$(CC) -fPIC -c -Os library.c -o library.o
	$(CC) -Wl,-rpath=. -fPIC -shared -o library.so library.o

measure-static-Os:
	make static-Os
	$(CC) -Os time-check.c -o measure -L. library.a -I .
	echo "Time measured using static-lib and -Os flag:" >> raport3a.txt
	./measure raport3b.txt

measure-shared-Os:
	make shared-Os
	$(CC) -Os time-check.c -o measure -L. library.so -Wl,-rpath=. -I .
	echo "Time measured using shared-lib and default -Os flag:" >> raport3a.txt
	./measure raport3b.txt

measure-dynamic-Os:
	make dynamic-Os
	$(CC) -Os time-check.c -o measure -ldl -D DYNAMIC
	echo "Time measured using dynamic-lib and default -Os flag:" >> raport3a.txt
	./measure raport3b.txt

measure-default-for3b:
	make static
	$(CC) time-check.c -o measure -L. library.a -I .
	echo "Time measured using static-lib and default flags:" >> raport3b.txt
	./measure raport3b.txt
	make shared
	$(CC) time-check.c -o measure -L. library.so -Wl,-rpath=. -I .
	echo "Time measured using shared-lib and default flags:" >> raport3b.txt
	./measure raport3b.txt
	make dynamic
	$(CC) time-check.c -o measure -ldl -D DYNAMIC
	echo "Time measured using dynamic-lib and default flags:" >> raport3b.txt
	./measure raport3b.txt

create-raport3b: clean-3b measure-default-for3b measure-static-O3 measure-shared-O3 measure-dynamic-O3 measure-static-Os measure-shared-Os measure-dynamic-Os

clean: clean-libs clean-2 clean-3a clean-3b

clean-libs:
	rm -f *.o
	rm -f *.so
	rm -f *.a

clean-2:
	rm -f raport2.txt

clean-3a:
	rm -f raport3a.txt

clean-3b:
	rm -f raport3b.txt

