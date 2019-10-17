#include "mymalloc.h"

#define DEBUG_ACTIVE 0
#define HEAP_SIZE 4096

static char myblock[HEAP_SIZE];
int num_blocks = 0;



// START ALLOCATION FUNCTIONS

void* mymalloc(size_t user_size, int line, char* file) {
	
	// Finding an empty block with enough space.
	if(DEBUG_ACTIVE) printf("mymalloc(): calling find_block()...\n");
	metadata* meta_ptr = find_block(user_size);
        if(DEBUG_ACTIVE) printf("mymalloc(): ...cleared find_block()\n");
	if(meta_ptr == NULL) {
		return(NULL);
	}

	// Splitting the empty block into a used and unused part.
	if(DEBUG_ACTIVE) printf("mymalloc(): calling split_block()...\n");
	int split_block_error = split_block(meta_ptr, user_size);
	if(DEBUG_ACTIVE) printf("mymalloc(): ...cleared split_block()\n");
	if(split_block_error) {
		return(NULL);
	}

	// Producting a pointer to give to the user.
        if(DEBUG_ACTIVE) printf("mymalloc(): calling get_return_ptr()...\n");
	void* return_ptr = get_return_pointer(meta_ptr);
        if(DEBUG_ACTIVE) printf("mymalloc(): ...cleared get_return_ptr()\n");
	
	return(return_ptr);
}

metadata* find_block(size_t user_size) {
	int c0;	

	// If user_size larger than max short value.
	if(user_size > SHRT_MAX) {
		return(NULL);
	}
	
	// If this is the first block requested.
	if(num_blocks == 0) {
		
		// Initializing my_heap to a default value.
		for(c0=0; c0<HEAP_SIZE; ++c0) {
			myblock[c0] = 0;
		}

                if(DEBUG_ACTIVE) printf("find_block(): making first block...\n");
		metadata first_meta = {(short)(-1*(HEAP_SIZE-sizeof(metadata)))};
		*((metadata*)(myblock+HEAP_SIZE-sizeof(metadata))) = first_meta;
		num_blocks += 1;	
                if(DEBUG_ACTIVE) printf("find_block(): ...made first block\n");

	}

	// Looking through metadata for appropriate empty block.
	metadata* cur_meta;
	for(c0=HEAP_SIZE-sizeof(metadata); c0>=HEAP_SIZE-num_blocks*sizeof(metadata); c0-=sizeof(metadata)) {
		cur_meta = (metadata*)(myblock+c0);
		
                if(DEBUG_ACTIVE) printf("find_block():\tcur_meta:%hd user_size:%hd\n", cur_meta->size, (short)user_size);
		// If block is unused and big enough.
		if(cur_meta->size < 0 && cur_meta->size+(short)user_size < 0) {
			return(cur_meta);
		}	
	}

	// No unused blocks large enough.
	return(NULL);
}

int split_block(metadata* meta_ptr, size_t user_size) {
	int meta_index, c0;

	// Finding the location of the requested block.
	metadata* cur_meta;
	if(DEBUG_ACTIVE) printf("split_block(): searching for block...\n");
	for(meta_index=HEAP_SIZE-sizeof(metadata); meta_index>=HEAP_SIZE-num_blocks*sizeof(metadata); meta_index-=sizeof(metadata)) {
		cur_meta = (metadata*)(myblock+meta_index);
		if(cur_meta == meta_ptr) {
			if(DEBUG_ACTIVE) printf("split_block(): ...found block\n");
			break;
		}
	}
	// If the requested block was not found.
	if(meta_index < HEAP_SIZE-num_blocks*sizeof(metadata)) {
	        if(DEBUG_ACTIVE) printf("split_block(): ...did not find block\n");
		return(1);
	}

	// Shifting over existing blocks
	metadata temp_meta;
	for(c0=HEAP_SIZE-num_blocks*sizeof(metadata); c0<meta_index; c0+=sizeof(metadata)) {
		cur_meta = (metadata*)(myblock+c0);

		// If shift would place metadata out of bounds.
		if(c0 < 2) {
			return(2);
		}
		
		// Shifting the current metadata one spot back.
		temp_meta = *cur_meta;
		*((metadata*)(myblock+HEAP_SIZE-sizeof(metadata))) = temp_meta;
	}

	// Splitting the requested block.
        metadata new_meta = {(short)(meta_ptr->size+(short)user_size+sizeof(metadata))};
	*((metadata*)(myblock+meta_index-sizeof(metadata))) = new_meta;
	meta_ptr->size = (short)user_size;	
	
	num_blocks += 1;

	return(0);
}

void* get_return_pointer(metadata* meta_ptr) {
	int c0;	

	void* return_ptr = myblock;
	metadata* cur_ptr;
	for(c0=HEAP_SIZE-sizeof(metadata); c0>=HEAP_SIZE-num_blocks*sizeof(metadata); c0-=sizeof(metadata)) {
		cur_ptr = (metadata*)(myblock+c0);
		
		// Target metadata is associated with current return pointer.
		if(cur_ptr == meta_ptr) {
			return(return_ptr);
		}	

		// Move current return pointer to next available.
		return_ptr += cur_ptr->size;
        }

	return(NULL);
}

// END ALLOCATION FUNCTIONS



// START FREE FUNCTIONS

void myfree(void* user_ptr, int line, char* file) {

	// Marking the metadata associated with user_ptr as unused.
        if(DEBUG_ACTIVE) printf("myfree(): calling mark_unused()...\n");
	metadata* meta_ptr = mark_unused(user_ptr);
        if(DEBUG_ACTIVE) printf("myfree(): ...cleared mark_unused()\n");
	if(meta_ptr == NULL) {
		return;
	}

	// Stitching together adjancent free blocks.
        if(DEBUG_ACTIVE) printf("myfree(): calling stitch()...\n");
	int stitch_error = stitch();
	if(DEBUG_ACTIVE) printf("myfree(): ...cleared stitch()\n");
	if(stitch_error != 0) {
		return;
	}	

	return;
}

metadata* mark_unused(void* user_ptr) {
        int c0;

        metadata* cur_ptr;
	int data_index = 0;
        for(c0=HEAP_SIZE-sizeof(metadata); c0>=HEAP_SIZE-num_blocks*sizeof(metadata); c0-=sizeof(metadata)) {
                cur_ptr = (metadata*)(myblock+c0);

                // If target metadata is associated with current return pointer.
                if((metadata*)user_ptr == (metadata*)(myblock+data_index)) {
			
			// If user gave a valid pointer to free.
			if(cur_ptr->size > 0) {
				cur_ptr->size *= -1;
				return(cur_ptr);
		
			// If user gave pointer to a free block.
			} else {
				return(NULL);
			}
                }

		// Move on to the next block to compare.
		if(cur_ptr->size > 0) {
			data_index += cur_ptr->size;
		} else {
			data_index -= cur_ptr->size;
		}
        }

	// User did not give a valid pointer.
        return(NULL);
}

int stitch() {
        int c0, c1;
	
        metadata* cur_ptr;
	metadata* prev_ptr;
	metadata temp_meta;
        for(c0=HEAP_SIZE-2*sizeof(metadata); c0>=HEAP_SIZE-num_blocks*sizeof(metadata); c0-=sizeof(metadata)) {
                cur_ptr = (metadata*)(myblock+c0);
		prev_ptr = (metadata*)(myblock+c0+sizeof(metadata));	

                // If there are two sequential free blocks.
                if(prev_ptr->size <= 0 && cur_ptr->size <= 0) {

                        // Stitching together the two free blocks.
			prev_ptr->size += cur_ptr->size + sizeof(metadata);

			// Shifting over metadata.
			for(c1=c0-sizeof(metadata); c1>=HEAP_SIZE-num_blocks*sizeof(metadata); c1-=sizeof(metadata)) {
				temp_meta = *((metadata*)(myblock+c1));
				*((metadata*)(myblock+c1+sizeof(metadata))) = temp_meta;		
			}
			num_blocks -= 1;

			// Trying to stitch again.
			int stitch_result = stitch();
			
			return(stitch_result);
                }
        }

        // Nothing to stitch, but nothing broke.
        return(0);
}

// END FREE FUNCTIONS



// START DEBUGGING FUNCTIONS

void print_status() {
	int c0;

	int data_index = 0;
	metadata* cur_meta;
        for(c0=HEAP_SIZE-sizeof(metadata); c0>=HEAP_SIZE-num_blocks*sizeof(metadata); c0-=sizeof(metadata)) {
		cur_meta = (metadata*)(myblock+c0);

		
		printf("Block at %d:\t", data_index);
		if(cur_meta->size > 0) {
			printf("Used:1\t");
			printf("Size:%hd\n", cur_meta->size);
			data_index += cur_meta->size;
		} else {
			printf("Used:0\t");
			printf("Size:%hd\n", -1*cur_meta->size);
			data_index -= cur_meta->size;
		}
	}

	return;
}

// END DEBUGGING FUNCTIONS
