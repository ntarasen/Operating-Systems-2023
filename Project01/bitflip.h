#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Functions */

void usage(int status);
int byteFlip(char* buff, size_t size1);
char* readfile(FILE* fp, size_t size1);
void writefile(char* outfile, char* buff, size_t size1);
void reverse(char* buff, size_t size1);

