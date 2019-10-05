#include <stdio.h>
#include "mymalloc.h"

// Function Definitions
void* mymalloc(int space, int line, char* file) {
	
	printf("Error on line %d in file %s\n", line, file);

}
