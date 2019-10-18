#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"

const int N_TESTS = 10;
const int LOCAL_RAND_MAX = 64;

int main (int argc, char *argv[])
{
        printf("\nSTARTING MALLOC:\n");
	      char* ptr1 = malloc(sizeof(char) * 1);
        char* ptr2 = malloc(sizeof(char) * 64);
        char* ptr3 = malloc(sizeof(char) * 2);
        char* ptr4 = malloc(sizeof(char) * 128);
        char* ptr5 = malloc(sizeof(char) * 4);
        char* ptr6 = malloc(sizeof(char) * 256);
	      print_status();
	
	      printf("\nSTARTING FREE:\n");
	      free(ptr2);
        free(ptr3);
        free(ptr6);
        print_status();

	      return(0);
}
