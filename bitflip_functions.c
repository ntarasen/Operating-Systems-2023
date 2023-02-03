#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "bitflip.h"

void usage(int status) {
	fprintf(stderr, "Usage: ./bitflip arguments...\n");
	fprintf(stderr, "   -o XXX     overide of the output file name to use\n");
	fprintf(stderr, "   -maxsize XXX     override of the output file sie allowed for input where XXX is the new maximum file size in bytes\n");
	fprintf(stderr, "   -bfr     Reverse the order of the bytes and bit-flip the bits in the file\n");
	fprintf(stderr, "   -r     Reverse the order of the bytes in the file without doing a bitflip and use a default extension of .r\n");
	exit(status);
}

char* readfile(FILE* fp, size_t size1) {
	char *buff = malloc(size1); //returning a char* cannot be stack must be heap
	size_t numbytes = size1;
	int index = 0;
	while (numbytes > 0) { //this while loop is because fread is not guarenteed to read everything at once
		size_t numread = fread(buff, 1, numbytes, fp);
		if (numread <= 0) {
			perror("fread");
			exit(1);
		}
		numbytes -= numread;//decrement numbytes by the number of bytes that were read
		index += numread;   //increment index by the number of bytes that were read
	}
	return buff; //returns the address of the buff from the heap
}
void writefile(char* outfile, char* buff, size_t size1) {
	struct stat statbuff;
	
	if (stat(outfile, &statbuff) != -1) { //We are using stat to see if the file already exists. When the stat does not fail then the file exists
		fprintf(stderr, "Error: %s already exists!\n", outfile);
		exit(1);
	}
	FILE* fpout = fopen(outfile, "w");
	if (fpout == NULL) { //check if there is something wrong with file
		perror("fopen");
		exit(1);
	}
	size_t numbytes = size1;
	size_t index = 0;
	while (numbytes > 0) {
		size_t r = fwrite(&buff[index], 1, numbytes, fpout);
		if (r <= 0) {
			perror("fwrite");
			exit(1);
		}   
		numbytes -= r; //keeps track of how many bytes are remaining to be written
		index += r; //Keeps track of where we are in the buffer
	}
	fclose(fpout);

}
void reverse(char* buff, size_t size1) {
	for (int i = 0; i < size1/2; i++) { //start by swapping first and last elements the first element is at i and the last element is at size1-i-1. As i increases we will start swapping the second element with the second to last element and the third element with the third to last element. We stop swapping when we get to the midpoint.
		char temp = buff[i];
		buff[i] = buff[size1-i-1];
		buff[size1-i-1] = temp;
	}
}
int byteFlip(char* buff, size_t size1) {

	int byteVal = 0;
	int num = 0;
	for (int i = 0; i < size1; i++) {
		num = buff[i];  
		byteVal = num ^ 0xFF;
		buff[i] = byteVal;
	}   

	return 0;
}


