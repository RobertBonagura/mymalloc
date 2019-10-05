#ifndef _MYMALLOC_H
#define _MYMALLOC_H

#define malloc(x) mymalloc((x), __LINE__, __FILE__)

typedef struct _metadata {
	
	short size;
	char used;
	short prev;
	short next;
} metadata;

void* mymalloc(int space, int line, char* file);
void myfree(short index);
void stitch(short prev_index, short next_index);


#endif
