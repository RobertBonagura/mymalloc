#ifndef _MYMALLOC_H
#define _MYMALLOC_H

#define malloc(x) mymalloc((x), __LINE__, __FILE__)

#ifndef _METADATA_STRUCT
#define _METADATA_STRUCT

typedef struct _metadata {
	short size;
	char used;
	short prev;
	short next;
} metadata;

#endif


void* mymalloc(int space, int line, char* file);

#endif
