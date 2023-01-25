#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


int main(int argc, char* argv[]) {
	//makes sure the num of arguments is ok
	if (argc != 3) {
		fprintf(stderr, "Usage: %s filename 0xNN\n", argv[0]);
		exit(1);
	}

	//Makes sure that hex val is put into command line properly
	if (strlen(argv[2]) < 3) {
		fprintf(stderr, "Usage: %s filename 0xNN\n", argv[0]);
		exit(1);	
	}

	/* Making sure that hex val is put in properly and using the strtol func to turn the hex num to dec  */
	int hexnum = 0;
	if (argv[2][0] == '0' && tolower(argv[2][1]) == 'x') {
		hexnum = strtol((argv[2]+2), NULL, 16);
	} else {
		fprintf(stderr, "Usage: %s filename 0xNN\n", argv[0]);
		return 1;
	}


	FILE *fp;
	char buff[25000]; //Buffer needs to read largest possible filesize
	fp = fopen(argv[1], "r");
	if (fp == NULL) { //check if there is something wrong with file
		perror("fopen");
		exit(1);
	}

	//checking how large the file is
	fseek(fp, 0L, SEEK_END);
	int size = ftell(fp);
	rewind(fp);

	//If the file is greater than 25KB the ERROR
	if (size > 25000) {
		fprintf(stderr, "Error: The file is over 25 kB (the file size was %d)\n", size);
		exit(1);
	}

	//catching return val of fread to see how many bytes fread read
	size_t numread = fread(buff, 1, size, fp);

	int count = 0;
	for (int i = 0; i < numread; i++) {
		if (buff[i] == hexnum) {//checking if the ascii matches
			count++;
		}
	}

	printf("%d\n", count);

	fclose(fp); //close fp
	return 0;
}

