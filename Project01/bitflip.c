#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "bitflip.h"

int main(int argc, char* argv[]) {
	if (strcmp(argv[1], "-help") == 0) {
		usage(0);
	}

	char infile[1024] = ""; //This is the name of the file the user inputs



	int maxsize = 25000;


	char outfile[1024] = "";//buffer for new file name (file we make with extension)


	bool bf = true;
	bool rev = false;

	char extension[5] = ".bf";

	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-o") == 0) {
			if ((i+1) >= argc) {//Checks to see if there is an argument after -o
				usage(1);
			}

			int length = strlen(argv[i+1]); //finding length of the new file name user passed
			strncpy(outfile, argv[i+1], length+1); //user input filename into outfile array
			i++; //we have to increment i to skip over the outfile name
		} else if (strcmp(argv[i], "-maxsize") == 0) {
			if ((i+1) >= argc) {//checks if there is a parameter after -maxsize
				fprintf(stderr, "Error: Valid number not specified for -maxsize\n");
				exit(1);
			}
			//checking if the string after maxsize is a number or not
			for (int j = 0; j < strlen(argv[i+1]); j++) {
				int tmp = argv[i+1][j];
				if (!isdigit(tmp)) {//looks for a num
					fprintf(stderr, "Error: Valid number not specified for -maxsize\n");
					exit(1);
				}
			} 

			maxsize = strtol(argv[i+1], NULL, 10); //converts string to num
			i++;
		} else if (strcmp(argv[i], "-r") == 0) {
			bf = false;//Dont bitflip
			rev = true;//reverse
			if (strcmp(extension, ".bfr") == 0) {//if the user has already did -bfr its an error
				fprintf(stderr, "Options -r and -bfr are not allowed at the same time\n");
				exit(1);
			}
			strncpy(extension, ".r", 3);			

		} else if (strcmp(argv[i], "-bfr") == 0) {
			bf = true;
			rev = true;
			if (strcmp(extension, ".r") == 0) {
				fprintf(stderr, "Options -r and -bfr are not allowed at the same time\n");
				exit(1);
			}
			strncpy(extension, ".bfr", 5);			
		} else if (argv[i][0] == '-') {//checks if there is an argument that is not valid
			usage(1);
		} else {
			if (strcmp(infile, "") == 0) {// If we have not already specified a filename save it in infile
				strncpy(infile, argv[i], strlen(argv[i]) + 1);
			} else {
				usage(1);//specifying multiple infile names
			}
		}

	}
	if (strcmp(outfile, "") == 0) { //We never got a -o so we need to use the input filename
		strncpy(outfile, infile, strlen(infile) + 1);
		strncat(outfile, extension, strlen(extension)+1);
	}


	if (strcmp(outfile, infile) == 0) {//This is if the outfile name and the infile name are the same
		fprintf(stderr, "Error: %s already exists!\n", outfile);
		exit(1);
	}

	FILE *fp; 
	fp = fopen(infile, "r");
	if (fp == NULL) { //check if there is something wrong with file
		fprintf(stderr, "Error: The file %s does not exist or is not accessible.\n", infile);
		exit(1);
	}
	//checking how large the file is
	fseek(fp, 0L, SEEK_END);
	size_t size = ftell(fp);
	rewind(fp);
	
	if (size > maxsize) {//making sure size is less than maxsize
		fprintf(stderr, "Error: The file is over %d Bytes (the file size was %ld)\n", maxsize, size);
		exit(1);
	}

	char* buff = readfile(fp, size);


	if (bf) {
		byteFlip(buff, size);

	}
	if (rev) {
		reverse(buff, size);
	}
	writefile(outfile, buff, size);

	printf("Input: %s was %ld bytes\n", infile, size);
	printf("Output: %s was output successfully\n", outfile);

	fclose(fp); //close fp
	free(buff);
	return 0;
}

