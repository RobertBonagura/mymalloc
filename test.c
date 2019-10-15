#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"

const int N_TESTS = 10;
const int LOCAL_RAND_MAX = 64;

int main (int argc, char *argv[])
{

	printf("\nSTARTING MALLOC:\n\n");
	
	char* ptr1 = malloc(sizeof(char) * ((rand() % LOCAL_RAND_MAX) + 1));
    char* ptr2 = malloc(sizeof(char) * ((rand() % LOCAL_RAND_MAX) + 1));
    char* ptr3 = malloc(sizeof(char) * ((rand() % LOCAL_RAND_MAX) + 1));
    char* ptr4 = malloc(sizeof(char) * ((rand() % LOCAL_RAND_MAX) + 1));
    char* ptr5 = malloc(sizeof(char) * ((rand() % LOCAL_RAND_MAX) + 1));
    char* ptr6 = malloc(sizeof(char) * ((rand() % LOCAL_RAND_MAX) + 1));

/*	
	char* ptr1 = malloc(543);
    //char* ptr2 = malloc(49);
    //char* ptr3 = malloc(69);
	//char* ptr4 = malloc(67);
    //char* ptr5 = malloc(29);
    //char* ptr6 = malloc(859);
*/	
	showMeta((metadata*)ptr1);
	
	printf("\nSTARTING FREE:\n\n");

	free(ptr1);
	free(ptr2);
	//free(ptr3);
	free(ptr4);
    free(ptr6);
	
    showMeta((metadata*)ptr1);
	return 0;

}
