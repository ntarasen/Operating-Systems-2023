# -*- Makefile -*-

FLAGS = -Wall -g

singleshell: singleshell.o
	gcc $(FLAGS) singleshell.o -o singleshell

singleshell.o: singleshell.c 
	gcc $(FLAGS) -c singleshell.c

clean:
	rm *.o singleshell
