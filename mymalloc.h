#ifndef _MYMALLOC_H
#define _MYMALLOC_H
#include <stdio.h>
#include <limits.h>

#define malloc(x) mymalloc((x), __LINE__, __FILE__)
#define free(x) myfree((x), __LINE__, __FILE__)

/*
The size attribute of metadata is a signed short, conveying information about block size and usage.
block size: magnitude of metadata.size. 
block used: sign of metadata.size; negative is unused, positive is used.
*/
typedef struct _metadata {	
	short size;
} metadata;

typedef enum _DetectableError {FreeA, FreeB, FreeC, MallocA} DetectableError;

// Allocation functions:
void* mymalloc(size_t user_size, int line, char* file);
metadata* find_block(size_t user_size);
void* split_block(metadata* meta_ptr, size_t user_size);

// Free functions:
void myfree(void* user_ptr, int line, char* file);
metadata* mark_unused(void* user_ptr);
int stitch();

// Debugging functions:
void print_status();

#endif
