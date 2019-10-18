#include "mymalloc.h"

#define DEBUG_ACTIVE 1
#define HEAP_SIZE 4096


static char myblock[HEAP_SIZE];
int num_blocks = 0;



// START ALLOCATION FUNCTIONS

void* mymalloc(size_t user_size, int line, char* file) {

	// Finding an empty block with enough space.
	metadata* meta_ptr = find_block(user_size);

	// Splitting the empty block into a used and unused part.
	void* return_pointer = split_block(meta_ptr, user_size);
	if(return_pointer == NULL) {
		printf("mymalloc() error on line %d in file %s.\n", line, file);
	}

	return(return_pointer);
}

metadata* find_block(size_t user_size) {
	int c0;	

	if(DEBUG_ACTIVE) printf("find_block(): ");

	// If user_size larger than max short value.
	if(user_size > SHRT_MAX) {
		if(DEBUG_ACTIVE) printf("user size too large.\n");
		return(NULL);
	}
	if(user_size <= 0) {
		if(DEBUG_ACTIVE) printf("user size negative or zero.\n");
		return(NULL);
	}
	
	// If this is the first block requested.
	if(num_blocks == 0) {
		
		// Initializing my_heap to a default value.
		for(c0=0; c0<HEAP_SIZE; ++c0) {
			myblock[c0] = 0;
		}

                if(DEBUG_ACTIVE) printf("making first block...");
		metadata first_meta = {(short)(-1*(HEAP_SIZE-sizeof(metadata)))};
		*((metadata*)(myblock)) = first_meta;
		num_blocks += 1;	
                if(DEBUG_ACTIVE) printf("made first block...");

	}


	// Looking through metadata for appropriate empty block.
	if(DEBUG_ACTIVE) printf("searching for at least %d unused...", (short)user_size);
	metadata* cur_meta;
	int heap_index = 0;
	for(c0=0; c0<num_blocks; ++c0) {
		cur_meta = (metadata*)(myblock+heap_index);
		
		// If block is unused and big enough.
		if((cur_meta->size < 0) && (-1*cur_meta->size > (short)user_size+sizeof(metadata))) {
			if(DEBUG_ACTIVE) printf("found %d unused at %d\n", -1*(int)cur_meta->size, heap_index);
			return(cur_meta);
		}
		
		// Moving to the index of the next metadata.
		if(cur_meta->size < 0) {
			heap_index -= cur_meta->size;
		} else {
			heap_index += cur_meta->size;
		}
		heap_index += sizeof(metadata);
	}

	// No unused blocks large enough.
	if(DEBUG_ACTIVE) printf("no appropriate blocks found\n");
	return(NULL);
}

void* split_block(metadata* meta_ptr, size_t user_size) {
	int c0;

	// Finding the location of the requested block.
	metadata* cur_meta;
	int heap_index = 0;
	if(DEBUG_ACTIVE) printf("split_block(): searching for block...");
	for(c0=0; c0<num_blocks; ++c0) {
		cur_meta = (metadata*)(myblock+heap_index);
		
		// If block is found.
		if(cur_meta == meta_ptr) {
			if(DEBUG_ACTIVE) printf("found block at %d\n", heap_index);
			break;
		}

		// Moving to next metadata.
                if(cur_meta->size < 0) {
                        heap_index -= cur_meta->size;
                } else {
                        heap_index += cur_meta->size;
                }
                heap_index += sizeof(metadata);
	}

	// If the requested block was not found.
	if(c0 == num_blocks) {
	        if(DEBUG_ACTIVE) printf("did not find block\n");
	        return(NULL);
	}

	// Splitting the requested block.
        metadata new_meta = {(short)(meta_ptr->size+(short)user_size+sizeof(metadata))};
	*((metadata*)(myblock+heap_index+(short)user_size+sizeof(metadata))) = new_meta;
	meta_ptr->size = (short)user_size;	
	num_blocks += 1;
	
	// Returning a pointer for the user.
	return((void*)((char*)meta_ptr)+sizeof(metadata));
}

// END ALLOCATION FUNCTIONS



// START FREE FUNCTIONS

void myfree(void* user_ptr, int line, char* file) {

	// Marking the metadata associated with user_ptr as unused.
	metadata* meta_ptr = mark_unused(user_ptr);
	if(meta_ptr == NULL) {
		printf("myfree() error on line %d in file %s.\n", line, file);
		return;
	}

	// Stitching together adjancent free blocks.
	stitch();

	return;
}

metadata* mark_unused(void* user_ptr) {
        int c0;
	
	if(DEBUG_ACTIVE) printf("mark_unused(): checking user pointer...");
	metadata* user_meta = (metadata*)(((char*)user_ptr)-sizeof(metadata));
        metadata* cur_ptr;
	int heap_index = 0;
        for(c0=0; c0<num_blocks; ++c0) {
                cur_ptr = (metadata*)(myblock+heap_index);

                // If target metadata is associated with current return pointer.
                if(user_meta == cur_ptr) {
			
			// If user gave a valid pointer to free.
			if(cur_ptr->size > 0) {
				if(DEBUG_ACTIVE) printf("block of size %d marked unused.\n", cur_ptr->size);
				cur_ptr->size *= -1;
				return(cur_ptr);
		
			// If user gave pointer to a free block.
			} else {
				if(DEBUG_ACTIVE) printf("pointer to already free block.\n");
				return(NULL);
			}
                }

		// Move on to the next block to compare.
		if(cur_ptr->size < 0) {
			heap_index -= cur_ptr->size;
		} else {
			heap_index += cur_ptr->size;
		}
		heap_index += sizeof(metadata);
        }

	// User did not give a valid pointer.
	if(DEBUG_ACTIVE) printf("did not find metadata associated with pointer.\n");
        return(NULL);
}

int stitch() {
        int c0;
	
        metadata* cur_meta;
	metadata* next_meta;
        int heap_index = 0;
        if(DEBUG_ACTIVE) printf("stitch(): searching for blocks to stitch...");
        for(c0=0; c0<num_blocks-1; ++c0) {
                cur_meta = (metadata*)(myblock+heap_index);
		if(cur_meta->size < 0) {
			next_meta = (metadata*)(myblock+heap_index-cur_meta->size+sizeof(metadata));
		} else {
			next_meta = (metadata*)(myblock+heap_index+cur_meta->size+sizeof(metadata));
		}

                // If two unused blocks found.
                if((cur_meta->size < 0) && (next_meta->size < 0)) {

			if(DEBUG_ACTIVE) printf("stitching unused blocks of size %d and %d.\n", -1*cur_meta->size, -1*next_meta->size);

			cur_meta->size += next_meta->size;
			cur_meta->size -= sizeof(metadata);
			num_blocks -= 1;

			return(stitch());
                }

                // Moving to next metadata.
                if(cur_meta->size < 0) {
                        heap_index -= cur_meta->size;
                } else {
                        heap_index += cur_meta->size;
                }
                heap_index += sizeof(metadata);
        }

	if(DEBUG_ACTIVE) printf("no stitch opportunities found.\n");
	return(0);
	
}

// END FREE FUNCTIONS



// START DEBUGGING FUNCTIONS

void print_status() {
	int c0;

	int heap_index = 0;
	metadata* cur_meta;
	printf("\n\nmyblock status: %d blocks\n", num_blocks);
        for(c0=0; c0<num_blocks; ++c0) {
		cur_meta = (metadata*)(myblock+heap_index);
		
		printf("Block at %d:\t", heap_index);
		if(cur_meta->size > 0) {
			printf("Used:1\t");
			printf("Size:%hd\n", cur_meta->size);
		} else {
			printf("Used:0\t");
			printf("Size:%hd\n", -1*cur_meta->size);
		}
        	
		// Moving to next metadata.
       		if(cur_meta->size < 0) {
        	        heap_index -= cur_meta->size;
	        } else {
                	heap_index += cur_meta->size;
       		}
	        heap_index += sizeof(metadata);

	}
	printf("\n");

	return;
}

// END DEBUGGING FUNCTIONS
