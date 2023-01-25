#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


int main(int argc, char* argv[]) {

	if (argc != 3) {
		fprintf(stderr, "Usage: %s filename 0xNN\n", argv[0]);
		exit(1);
	}
	if (strlen(argv[2]) < 3) {
		fprintf(stderr, "Usage: %s filename 0xNN\n", argv[0]);
		exit(1);	
	}
	int hexnum = 0;
	if (argv[2][0] == '0' && tolower(argv[2][1]) == 'x') {
		hexnum = strtol((argv[2]+2), NULL, 16);
	} else {
		fprintf(stderr, "Usage: %s filename 0xNN\n", argv[0]);
		return 1;
	}


	FILE *fp;
	char buff[25000]; 
	fp = fopen(argv[1], "r");
	if (fp == NULL) {
		perror("fopen");
		exit(1);
	}
	fseek(fp, 0L, SEEK_END);
	int size = ftell(fp);
	rewind(fp);


	if (size > 25000) {
		fprintf(stderr, "Error: The file is over 25 kB (the file size was %d)\n", size);
		exit(1);
	}
	size_t numread = fread(buff, 1, size, fp);

	int count = 0;
	for (int i = 0; i < numread; i++) {
		if (buff[i] == hexnum) {
			count++;
		}
	}

	printf("%d\n", count);

	fclose(fp);
	return 0;
}

