# -*- Makefile -*-

bytecheck: bytecheck.o
	gcc bytecheck.o -o bytecheck -Wall

bytecheck.o: bytecheck.c
	gcc -c bytecheck.c -Wall
