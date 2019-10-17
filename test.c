#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"

const int N_TESTS = 10;
const int LOCAL_RAND_MAX = 64;

int main (int argc, char *argv[])
{



        printf("\nSTARTING MALLOC:\n\n");

	char* ptr1 = malloc(sizeof(char) * (rand() % LOCAL_RAND_MAX));
        char* ptr2 = malloc(sizeof(char) * (rand() % LOCAL_RAND_MAX));
        char* ptr3 = malloc(sizeof(char) * (rand() % LOCAL_RAND_MAX));
        char* ptr4 = malloc(sizeof(char) * (rand() % LOCAL_RAND_MAX));
        char* ptr5 = malloc(sizeof(char) * (rand() % LOCAL_RAND_MAX));
        char* ptr6 = malloc(sizeof(char) * (rand() % LOCAL_RAND_MAX));

	print_status();


	
	printf("\nSTARTING FREE:\n\n");

	free(ptr2);
        free(ptr3);
        free(ptr6);

        print_status();

	return 0;
}








