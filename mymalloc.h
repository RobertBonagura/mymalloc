#ifndef _MYMALLOC_H
#define _MYMALLOC_H

#define METADATA_SIZE  11
#define malloc(x) mymalloc((x), __LINE__, __FILE__)
#define free(x) myfree((x), __LINE__, __FILE__)

typedef struct _metadata {
	
	short size;
	char used;
	struct _metadata* prev;
	struct _metadata* next;
} metadata;


void* mymalloc(int size, int line, char* file);
void* myfree(void* ptr, int line, char* file);

#endif
