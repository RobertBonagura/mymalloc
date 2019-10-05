#ifndef _MYMALLOC_H
#define _MYMALLOC_H

#define malloc(x) mymalloc((x), __LINE__, __FILE__)

void* mymalloc(int space, int line, char* file);

#endif
