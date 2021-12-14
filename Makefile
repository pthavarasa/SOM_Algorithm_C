run:
	gcc -c -Wall main.c
	gcc -c -Wall som.c
	gcc -o som main.o som.o -lm
	rm *.o

debug:
	gcc -c -Wall debug.c
	gcc -c -Wall som.c
	gcc -o debug debug.o som.o -lm
	rm *.o