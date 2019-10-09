#include <stdio.h>
#include "mymalloc.h"

static char myblock[4096];
int isFirst = 1;


void showMeta(metadata* head){
	
	metadata* current = head;
	int byteCount = 0;
	int prevSize = 0;
	while (byteCount < 4096){
		metadata meta = *current;
		printf("---\n");
		printf("This metaPtr is stored at: %d\n", current);
		printf("---\n");
		printf("The size is: %d\n", current->size);
		printf("This metadata is in use: %c\n", current->used);
		printf("This metadata prev is stored at: %d\n", current->prev);
		//printf("current - currentPrev = %d\n", current - current->prev);
		printf("This metadata next is stored at: %d\n", current->next);
		printf("currentNext - current = %d\n\n", current->next - current);
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
	*(metadata*)(myblock) = meta;
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
metadata* updateMeta(metadata* metaPtr, int size, int prevMetaDistance, int byteCount){
	
	metadata* prevPtr = metaPtr - prevMetaDistance;	
	printf("prevPtr is at %d\nWhich should be %d less than metaPtr\n", prevPtr, prevMetaDistance);	
	short nextSize = 4096 - byteCount;	// check if nextSize is negative
	metadata next = {nextSize, '0', metaPtr, NULL};
	metadata* nextPtr = (metadata*)(metaPtr + size + METADATA_SIZE); 
	*(nextPtr) = next;

	metadata meta = {size, '1', prevPtr, nextPtr};
	*metaPtr = meta;
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
		int prevSize = 0;
		int byteCount = 0;
		while (current->used == '1' || current->size < size){
			prevSize = current->size;
			byteCount = byteCount + METADATA_SIZE + prevSize;
			current = current->next;
		}
		int prevMetaDistance = prevSize + METADATA_SIZE;
		byteCount = byteCount + METADATA_SIZE + size;
		metadata* newCurrent = 
			updateMeta(current, size, prevMetaDistance, byteCount);	
		return newCurrent;
	}

}

void* mymalloc(int size, int line, char* file) {

	metadata* metaPtr = (metadata*) getNextMetadata(size);
	metadata meta = *metaPtr;
	
	if (size <= meta.size){
		void* ptr = metaPtr;
		return ptr;
	} else {
		printf("Error on line %d in file %s\n", line, file);
		return;
	} 
}
