#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"

int main (int argc, char *argv[])
{
	char* ptr1 = (char*) malloc(sizeof(char) * 89);
	char* ptr2 = (char*) malloc(sizeof(char) * 89);
	char* ptr3 = (char*) malloc(sizeof(char) * 89);
	char* ptr4 = (char*) malloc(sizeof(char) * 89);
	
	showMeta((metadata*)ptr1);
	
	return 0;
}








