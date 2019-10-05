#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"

int main (int argc, char *argv[])
{
	char* ptr = (char*) malloc(sizeof(char) * 100);
	printf("make works\n");
	ptr[0] = 'h';
	ptr[1] = 'i';
	ptr[2] = '\0';

	printf("%s\n", ptr);		
	
	return 0;
}








