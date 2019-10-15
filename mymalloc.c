#include <stdio.h>
#include "mymalloc.h"

static char myblock[4096];
int isFirst = 1;

#define I_AM_DEBUGGING 0

//
//	showMeta - Prints metadata information for given metaPtr, and continues
//	iterating through next values.
//
void showMeta(metadata* head){
	
	printf("metadata: %d\n", sizeof(metadata));	
	printf("char: %d\n", sizeof(char));	
	printf("metaPTr: %d\n", sizeof(metadata*));	
	printf("short: %d\n", sizeof(short));	
	metadata* current = head;
	int byteCount = 0;
	int prevSize = 0;
	while (byteCount < 4096){
		metadata meta = *current;
		printf("---\n");
		printf("This metaPtr is stored at: %p\n", current);
		printf("---\n");
		printf("The size is: %d\n", current->size);
		printf("This metadata is in use: %c\n", current->used);
		printf("This metadata prev is stored at: %p\n", current->prev);
		printf("This metadata next is stored at: %p\n", current->next);
		prevSize = current->size;
		byteCount = byteCount + METADATA_SIZE + prevSize;
		current = current->next;
	}

}



//
//	makeFirst - creates first metadata value based on given size needed, and
//	writes it to the static array.
//
//	1. metadata value is initially written to static array not containing any
//	pointers.
//	2. sizeLeft for next metadata value is calculated, and then this value is
//	created, stored in the array, and then addressed by a pointer used by
//	metaPtr->next.
//	3. Global isFirst flag is turned off and the new pointer is returned.
//
metadata* makeFirst(int size){
	
	metadata meta = {size, '1', NULL, NULL};
	*((metadata*)(myblock)) = meta;
	metadata* metaPtr = (metadata*) myblock;

	short sizeLeft = 4096 - size - (2 * METADATA_SIZE);
	metadata next = {sizeLeft, '0', metaPtr, NULL}; 
	*(metadata*)(myblock + METADATA_SIZE + size) = next;
	metadata* nextPtr = (metadata*)(myblock + METADATA_SIZE + size);
	metaPtr->next = nextPtr;
	isFirst = 0;
	return metaPtr;
}




//
//	UpdateMeta - Takes metadata ptr and returns it with accurate size, 
//					used, prev and next values.

//	1. Uses pointer arithmetic to determine previous meta ptr
//	2. Creates next meta and intializes it to {'rest of array', unused, metaPtr, NULL.}  
//	3. Writes next value and meta value to array by derferencing pointer
//	
//	metadata* metaPtrb - the pointer to metadata in char array
//	int size - the size of this metadata
//	in prevMetaDistance - the number of bytes the previous meta is from this one
//	int byteCount - the 1-based index of the metadata in the char array 
//					(4096 - bytecount) represents the number of bytes remaining.
//					This assumes unused metadata has no data beyond it.
//
//
metadata* updateMeta(metadata* metaPtr, int size){
	
	short nextSize = metaPtr->size - METADATA_SIZE - size;
	metadata newmeta = {nextSize, '0', metaPtr, metaPtr->next};
	*((metadata*) (((char*) metaPtr) + METADATA_SIZE + size)) = newmeta;
	metadata* newPtr = (metadata*) (((char*) metaPtr) + METADATA_SIZE + size);
	metaPtr->next = newPtr;
	if (newPtr->next != NULL){
		newPtr->next->prev = newPtr;
	}
	metaPtr->size = size;
	metaPtr->used = '1';
	return metaPtr;
}



//
//	getNextMetadata - Loops through metadata stored in static array, untill
//	available metadata of the appropriate size is found.
//	If this is the first time mymalloc is called, makeFirst is called.
//	Otherwise, updating recent most metadata value.
//
//
metadata* getNextMetadata(int size){
	
	if (isFirst == 1){
		metadata* metaPtr = makeFirst(size);
		return metaPtr;
	} else {
		metadata* current = (metadata*) myblock;
		while (current->next != NULL || current->used == '1' || current->size <= size){
			if(I_AM_DEBUGGING) printf("\ttraversal start...\n");
			current = current->next;
            if(I_AM_DEBUGGING) printf("\t...traversal success\n");
		}
		if (current == NULL) {
			return(NULL);
		}
		if(I_AM_DEBUGGING) printf("\tcalling updateMeta()...\n");
		metadata* newCurrent = updateMeta(current, size);
        if(I_AM_DEBUGGING) printf("\t...cleared updateMeta()\n");
	
		return newCurrent;
	}

}

void* mymalloc(int size, int line, char* file) {

    if(I_AM_DEBUGGING) printf("\tcalling getNextMetadata()...\n");
	metadata* metaPtr = (metadata*) getNextMetadata(size);
    if(I_AM_DEBUGGING) printf("\t...cleared getNextMetadata()\n");
	metadata meta = *metaPtr;
	// Add to metadata pointer.
	
	if (size <= meta.size){
		void* ptr = metaPtr;
		return ptr;
	} else {
		printf("Error on line %d in file %s\n", line, file);
		return;
	}
	
	if(metaPtr == NULL) {
		printf("Dynamic memory saturation error on line %d in file %s\n", line, file);
		return(NULL);
	}
	
	void* ptr = metaPtr;
	return ptr; 
	

}


/*
isPointerValid()
metadata* ptr - user-given pointer to check for validity.
returns 1 if valid pointer, 0 otherwise.
*/
int isPointerValid(metadata* ptr) {
	metadata* cur = (metadata*) myblock;
	while(cur != NULL) {
		if(cur == ptr) {
			return(1);
		}
		cur = cur->next;
	}
	return(0);
}

/*
stitchFreeBlocks()
metadata* ptr - the first of a chain of free blocks.
returns the size of the conglomerated block.
*/
short stitchFreeBlocks(metadata* ptr) {
	if(ptr == NULL) {
		return(0);
	}
	if(ptr->used == '1') {
		return(0);
	}
	short newSize = ptr->size;
	metadata* cur = ptr->next;
	while(cur != NULL) {
		if (cur->used == '0'){
			printf("inside while\n");
			newSize += METADATA_SIZE + cur->size;
			cur = cur->next;
		}
		else break;
	}
	ptr->size = newSize;
	ptr->next = cur;
	if (cur != NULL) {
		cur->prev = ptr;
	}
	return(ptr->size); 
}

void myfree(void* ptr, int line, char* file) {
	printf("myfree() call\n");
	metadata* metaPtr = (metadata*) ptr;
	if (isPointerValid(ptr)) {
		printf("\tptr is valid\n");
		printf("\tCurrent pointer is %p, prev is %p\n", metaPtr, metaPtr->prev);
		metaPtr->used = '0';
		while (metaPtr != NULL){
			printf("\tin while\n");
			if ( metaPtr->prev != NULL) { // This line here needs to be fixed
				printf("\tprev is not null\n");
				if (metaPtr->prev == NULL){
					break;
				}
				printf("\tprev is %p, and NULL is %p\n", metaPtr->prev, NULL);
				if (metaPtr->prev->used == '0'){
					metaPtr = metaPtr->prev;
				}
				else break;
			}
			else break;
        }
		printf("\t%d\n", metaPtr);
        if(metaPtr == NULL) {
			return;
	    }
		printf("\tfirst free block size %hi\n", metaPtr->size);
		metaPtr->size = stitchFreeBlocks(metaPtr);	
        printf("\tfirst free block size after %hi\n", metaPtr->size);
	
		return;
	}
	else {
    	printf("Invalid free() on line %d in file %s\n", line, file);
    	return;
	}	
}
