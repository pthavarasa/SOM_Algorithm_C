run:
	gcc -c -Wall main.c
	gcc -c -Wall som.c
	gcc -o som main.o som.o -lm
	rm *.o