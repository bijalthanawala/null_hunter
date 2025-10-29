CC=gcc
CFLAGS=-Wall -Wpedantic -std=c2x

all: null_hunter test_null_hunter

null_hunter: null_hunter.o main.o
	$(CC) -o null_hunter null_hunter.o main.o

test_null_hunter: null_hunter.o test_null_hunter.o
	$(CC) -o test_null_hunter null_hunter.o test_null_hunter.o -lcunit

null_hunter.o: null_hunter.c
	$(CC) $(CFLAGS) -c -o null_hunter.o null_hunter.c

main.o: main.c
	$(CC) $(CFLAGS) -c -o main.o main.c

test_null_hunter.o: test_null_hunter.c
	$(CC) $(CFLAGS) -c -o test_null_hunter.o test_null_hunter.c

check: test_null_hunter
	./test_null_hunter

coverage:
	gcc -O0 --coverage -o test_null_hunter_coverage null_hunter.c test_null_hunter.c -lcunit
	./test_null_hunter_coverage
	gcov test_null_hunter_coverage-null_hunter
	echo "View the file null_hunter.c.gcov for the details"

clean:
	if [ -e null_hunter.o ]; then rm null_hunter.o; fi;
	if [ -e main.o ]; then rm main.o; fi;
	if [ -e null_hunter ]; then rm null_hunter; fi;
	if [ -e test_null_hunter.o ]; then rm test_null_hunter.o; fi;
	if [ -e test_null_hunter ]; then rm test_null_hunter; fi;
	if [ -e test_null_hunter_coverage ]; then rm test_null_hunter_coverage; fi;
	find . -name "*.gcno" -delete
	find . -name "*.gcda" -delete
	find . -name "*.gcov" -delete
