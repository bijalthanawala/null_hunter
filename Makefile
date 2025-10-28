null_hunter: null_hunter.o main.o
	gcc -o null_hunter null_hunter.o main.o

test_null_hunter: null_hunter.o test_null_hunter.o
	gcc -o test_null_hunter null_hunter.o test_null_hunter.o -lcunit

null_hunter.o: null_hunter.c
	gcc -Wpedantic -std=c2x -c -o null_hunter.o null_hunter.c

main.o: main.c
	gcc -Wpedantic -std=c2x -c -o main.o main.c

test_null_hunter.o: test_null_hunter.c
	gcc -Wpedantic -std=c2x -c -o test_null_hunter.o test_null_hunter.c

all: null_hunter test_null_hunter

clean:
	if [ -e null_hunter.o ]; then rm null_hunter.o; fi;
	if [ -e main.o ]; then rm main.o; fi;
	if [ -e null_hunter ]; then rm null_hunter; fi;
	if [ -e test_null_hunter.o ]; then rm test_null_hunter.o; fi;
	if [ -e test_null_hunter ]; then rm test_null_hunter; fi;
