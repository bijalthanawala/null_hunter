null_hunter:
	gcc -o null_hunter null_hunter.c main.c

clean:
	if [ -e null_hunter.o ]; then rm null_hunter.o; fi;
	if [ -e main.o ]; then rm main.o; fi;
	if [ -e null_hunter ]; then rm null_hunter; fi;
