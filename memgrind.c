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
		printf("Average time for workload %c: %f milliseconds.\n", 65+i, (total_times[i]/100)/1000);
	}
	return(0);
}


double workload_a() {
	int i;
	char *ptr;	
	
	printf("Workload A: 150 cycles of allocating and immediately freeing 1 byte.\n");
	struct timeval start, end;
	gettimeofday(&start, NULL);
	printf("Working...\n");
	for(i=0; i<150; ++i) {
		ptr = malloc(sizeof(char));
		free(ptr);
	}
	gettimeofday(&end, NULL);
	printf("Workload A completed.\n");
	long seconds = (end.tv_sec - start.tv_sec);
	long micros = ((seconds * 1000000) + end.tv_usec) - (start.tv_usec);
	double result = micros;
	return result;
}

double workload_b() {
	int i, j;
	char *arr[50];

        printf("Workload B: 3 cycles of allocating 1 byte for each index in a 50-cell array, then freeing each index in the array.\n");
	struct timeval start, end;
	gettimeofday(&start, NULL);
	printf("Working...\n");
	for(i=0; i<3; ++i) {
		for(j=0; j<50; ++j) {
			arr[j] = malloc(sizeof(char));
		}
                for(j=0; j<50; ++j) {
                        free(arr[j]);
                }
	}
    printf("Workload B completed.\n");
	gettimeofday(&end, NULL);
	long seconds = (end.tv_sec - start.tv_sec);
	long micros = ((seconds * 1000000) + end.tv_usec) - (start.tv_usec);
	double result = micros;
	return result;
}

double workload_c() {
	int should_free, i_malloc, i_free;
	char *arr[50];
    printf("Workload C: Randomly choosing between allocating and freeing 1 byte until 50 allocations and frees have been performed.\n");
	printf("Working...\n");
	struct timeval start, end;
	gettimeofday(&start, NULL);
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
	gettimeofday(&end, NULL);
	long seconds = (end.tv_sec - start.tv_sec);
	long micros = ((seconds * 1000000) + end.tv_usec) - (start.tv_usec);
	double result = micros;
	return result;
}

double workload_d() {
        int should_free, malloc_sizes[50], total_used, i_malloc, i_free;
        char *arr[50];

        printf("Workload D: Randomly choosing between allocating and freeing between 1 and 64 bytes until 50 allocations and frees have been performed.\n");
	struct timeval start, end;
	gettimeofday(&start, NULL);
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
	gettimeofday(&end, NULL);
	long seconds = (end.tv_sec - start.tv_sec);
	long micros = ((seconds * 1000000) + end.tv_usec) - (start.tv_usec);
	double result = micros;
	return result;
}

double workload_e() {
	printf("Workload E: Fills heap with pointers, and then continues free and mallocing pointers 150 times. See testplan.txt for details.\n");
	struct timeval start, end;
	gettimeofday(&start, NULL);
	printf("Working...\n");
	
	// Initialize the arrays to NULL
	void* ptrs[4096];
	int ptrSizes[4096];
	int i_ptr;
	for (i_ptr = 0; i_ptr < 4096; i_ptr++){
		ptrs[i_ptr] = 0;
		ptrSizes[i_ptr] = 0;
	}

	// Store pointers untill malloc returns null
	i_ptr = 0;
	int randNum = (rand() % 64) + 1;
	void* ptr = malloc(sizeof(char) * randNum);
	while (ptr != NULL){
		ptrs[i_ptr] = ptr;
		ptrSizes[i_ptr] = randNum;
		i_ptr++;
		randNum = (rand() % 32) + 1;
		ptr = malloc(sizeof(char) * randNum);
	}
	while (ptr != NULL){
		ptrs[i_ptr] = ptr;
		ptrSizes[i_ptr] = randNum;
		i_ptr++;
		randNum = (rand() % 10) + 1;
		ptr = malloc(sizeof(char) * randNum);
	}
	while (ptr != NULL){
		ptrs[i_ptr] = ptr;
		ptrSizes[i_ptr] = randNum;
		i_ptr++;
		randNum = (rand() % 1) + 1;
		ptr = malloc(sizeof(char) * randNum);
	}
	int numPtrs = i_ptr;

	// Choose a pointer, free it, and then remalloc
	int i_reps;
	int size;
	for (i_reps = 0; i_reps < 150; i_reps++){
		randNum = (rand() % i_ptr);
		free(ptrs[randNum]);
		size = ptrSizes[randNum];
		ptr = malloc(sizeof(char) * size);
		ptrs[randNum] = ptr;
	}

	// Then free every pointer
	int i;
	for (i = 0; i < numPtrs; i++){
		free(ptrs[i]);
	}
	printf("Workload E completed\n");
	gettimeofday(&end, NULL);
	long seconds = (end.tv_sec - start.tv_sec);
	long micros = ((seconds * 1000000) + end.tv_usec) - (start.tv_usec);
	double result = micros;
	return result;
}

double workload_f() {
	printf("Workload F: Splits 50:50 chance of either mallocing space to create and print a string, followed by freeing that pointer OR randomly test one of the Detectable Errors 	described in the assignment sheet. This is done 150 times. \n");
	struct timeval start, end;
	gettimeofday(&start, NULL);
	printf("Working...\n");
	int i_reps;
	for (i_reps = 0; i_reps < 150; i_reps++){
		int fifty_fifty = rand() % 2;
		// Test malloc functionality by using pointer to print random string.
		if (fifty_fifty == 0){
			int randNum = (rand() % 64) + 1;
			char* ptr = (char*) malloc(sizeof(char) * randNum);
			int i;
			for (i = 0; i < randNum; i++){
				char randLetter = (rand() % 26) + 97;
				ptr[i] = randLetter;
			}
			ptr[i-1] = '\0';
			printf("%s\n", ptr);
			free(ptr);
		}
		// Test Detectable Errors described on assignment sheet.
		else {
			DetectableError testCase = (rand() % 4);
			int x;
			int *y;
			char* p;
			char* q;
			switch (testCase){
			case FreeA:
				free((int*) x);	 
				break;			
			case FreeB:
				free(y);
				break;
			case FreeC:
				p = (char*)malloc(100);
				free(p);
				free(p);
				break;
			case MallocA:
				q = (char*)malloc(4097);
				break;
			}
		}
	}
	printf("Workload F completed.\n");
	gettimeofday(&end, NULL);
	long seconds = (end.tv_sec - start.tv_sec);
	long micros = ((seconds * 1000000) + end.tv_usec) - (start.tv_usec);
	double result = micros;
	return result;
}


