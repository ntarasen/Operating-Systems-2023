# -*- Makefile -*-

FLAGS = -Wall -g

bitflip: bitflip.o bitflip_functions.o
	gcc $(FLAGS) bitflip.o bitflip_functions.o -o bitflip

bitflip.o: bitflip.c bitflip.h
	gcc $(FLAGS) -c bitflip.c

bitflip_functions.o: bitflip_functions.c
	gcc $(FLAGS) -c bitflip_functions.c

clean:
	rm *.o bitflip
