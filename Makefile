null_hunter:
	gcc -o null_hunter null_hunter.c

clean:
	if [ -e null_hunter.o ]; then rm null_hunter.o; fi;
	if [ -e null_hunter ]; then rm null_hunter; fi;
