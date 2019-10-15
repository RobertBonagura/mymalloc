#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mymalloc.h"

#ifndef METADATA_SIZE
#define METADATA_SIZE 7
#endif

double workload_a();
double workload_b();
double workload_c();
double workload_d();
double workload_e();
double workload_f();

int main(int argc, char **argv) {
	int i; 
	double total_times[6], a_times[100], b_times[100], c_times[100], d_times[100], e_times[100], f_times[100];
	
	for(i=0; i<6; ++i) {
		total_times[i] = 0;
	}
	for(i=0; i<100; ++i) {
		a_times[i] += workload_a();
		total_times[0] += a_times[i];
		b_times[i] += workload_b();
                total_times[1] += b_times[i];
		c_times[i] += workload_c();
                total_times[2] += c_times[i];
		d_times[i] += workload_d();
                total_times[3] += d_times[i];
		e_times[i] += workload_e();
                total_times[4] += e_times[i];
		f_times[i] += workload_f();
                total_times[5] += f_times[i];
	}
	for(i=0; i<6; ++i) {
		printf("Average time for workload %c: %f seconds.\n", 65+i, total_times[i]/100);
	}
	return(0);
}

double workload_a() {
	int i;
	char *ptr;	
	
	printf("Workload A: 150 cycles of allocating and immediately freeing 1 byte.\n");
	clock_t start = clock();
	printf("Working...\n");
	for(i=0; i<150; ++i) {
		ptr = malloc(sizeof(char));
		free(ptr);
		printf("malloc()ed and free()ed %d\n", i);
	}
	clock_t end = clock();
	printf("Workload A completed.\n");
	return((end - start)/(double)CLOCKS_PER_SEC);
}

double workload_b() {
	int i, j;
	char *arr[50];

        printf("Workload B: 3 cycles of allocating 1 byte for each index in a 50-cell array, then freeing each index in the array.\n");
        clock_t start = clock();
	printf("Working...\n");
	for(i=0; i<3; ++i) {
		for(j=0; j<50; ++j) {
			arr[i] = malloc(sizeof(char));
		}
                for(j=0; j<50; ++j) {
                        free(arr[i]);
                }
	}
	clock_t end = clock();
        printf("Workload B completed.\n");
        return((end - start)/(double)CLOCKS_PER_SEC);
}

double workload_c() {
	int should_free, i_malloc, i_free;
	char *arr[50];
	
        printf("Workload C: Randomly choosing between allocating and freeing 1 byte until 50 allocations and frees have been performed.\n");
        clock_t start = clock();
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
	clock_t end = clock();
	printf("Workload C completed.\n");
        return((end - start)/(double)CLOCKS_PER_SEC);
}

double workload_d() {
        int should_free, malloc_sizes[50], total_used, i_malloc, i_free;
        char *arr[50];

        printf("Workload D: Randomly choosing between allocating and freeing between 1 and 64 bytes until 50 allocations and frees have been performed.\n");
        clock_t start = clock();
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
	clock_t end = clock();
        printf("Workload D completed.\n");
        return((end - start)/(double)CLOCKS_PER_SEC);
}

double workload_e() {
	clock_t start = clock();
	clock_t end = clock();
        return((end - start)/(double)CLOCKS_PER_SEC);
}

double workload_f() {
        clock_t start = clock();
        clock_t end = clock();
        return((end - start)/(double)CLOCKS_PER_SEC);
}


