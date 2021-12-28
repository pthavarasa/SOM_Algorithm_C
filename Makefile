run:
	gcc -c -Wall -Wconversion -Werror -Wextra -Wpedantic main.c
	gcc -c -Wall -Wconversion -Werror -Wextra -Wpedantic som.c
	gcc -o som main.o som.o -lm
	rm *.o