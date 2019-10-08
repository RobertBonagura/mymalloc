#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"

#ifndef METADATA_SIZE
#define METADATA_SIZE 7
#endif

int workload_a();
int workload_b();
int workload_c();
int workload_d();
int workload_e();
int workload_f();

int main(int argc, char **argv) {
	int workload_a();
	int workload_b();
	int workload_c();
	int workload_d();
	int workload_e();
	int workload_f();
	return(0);
}

int workload_a() {
	int i;
	char *ptr;	
	
	printf("Workload A: 150 cycles of allocating and immediately freeing 1 byte.\n");
	printf("Working...\n");
	for(i=0; i<150; ++i) {
		ptr = malloc(sizeof(char));
		free(ptr);
	}
	printf("Workload A completed.\n");
	return(0);
}

int workload_b() {
	int i, j;
	char *arr[50];

        printf("Workload B: 3 cycles of allocating 1 byte for each index in a 50-cell array, then freeing each index in the array.\n");
        printf("Working...\n");
	for(i=0; i<3; ++i) {
		for(j=0; j<50; ++j) {
			arr[i] = malloc(sizeof(char));
		}
                for(j=0; j<50; ++j) {
                        free(arr[i]);
                }
	}
        printf("Workload B completed.\n");
        return(0);
}

int workload_c() {
	int should_free, i_malloc, i_free;
	char *arr[50];
	
        printf("Workload C: Randomly choosing between allocating and freeing 1 byte until 50 allocations and frees have been performed.\n");
        printf("Working...\n");
	i_malloc = 0;
	i_free = 0;
	while(i_malloc<50) {
                should_free = rand()%2;
		if(i_malloc==i_free || should_free==0) {
			arr[i_malloc] = malloc(sizeof(char));
			++i_malloc;
			continue;
		}
		free(arr[i_free]);
		++i_free;
	}
	while(i_free<50) {
		free(arr[i_free]);
		++i_free;
	}
	printf("Workload C completed.\n");
        return(0);
}

int workload_d() {
        int should_free, malloc_sizes[50], total_used, i_malloc, i_free;
        char *arr[50];

        printf("Workload D: Randomly choosing between allocating and freeing between 1 and 64 bytes until 50 allocations and frees have been performed.\n");
        printf("Working...\n");
        i_malloc = 0;
        i_free = 0;
	total_used = 0;
        while(i_malloc<50) {
                should_free = rand()%2;
                if((i_malloc==i_free || should_free==0) && total_used<4096) {
			malloc_sizes[i_malloc] = 1+rand()%64;
			// Accounting for metadata size.
			malloc_sizes[i_malloc] += METADATA_SIZE;
			// Not enough space, need to free.
			if(total_used+malloc_sizes[i_malloc]>4096) {
				free(arr[i_free]);
				total_used -= malloc_sizes[i_free];
				++i_free;
				continue;
			}
                        arr[i_malloc] = malloc((malloc_sizes[i_malloc]-METADATA_SIZE)*sizeof(char));
			total_used += malloc_sizes[i_malloc];
                        ++i_malloc;
                        continue;
                }
                free(arr[i_free]);
		total_used -= malloc_sizes[i_free];
                ++i_free;
        }
        while(i_free<50) {
                free(arr[i_free]);
                ++i_free;
        }
        printf("Workload D completed.\n");
        return(0);
}

int workload_e() {
        return(0);
}

int workload_f() {
        return(0);
}


