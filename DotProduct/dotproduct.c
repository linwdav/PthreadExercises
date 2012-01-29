/**
 * Pthread dot product.
 * @author David Lin
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>


/* Thread argument structure. */
typedef struct Struct {
	int *v1;
	int *v2;
	int start;
	int numComputation;
	int dotProduct;
} Struct;


/* Declare function(s). */
void *computeDotProduct(void *arg);


/* Execute program. */
int main(int argc, char *argv[]) {
	// Seed
	srand(time(NULL));
	
	// Check that there are two command-line arguments
	if (argc < 3) {
		printf("Usage: ./dotproduct <# of threads>\n");
		exit(0);
	}
	
	// Convert arguments to integers
	int numIntegers = strtol(argv[1], NULL, 10);
	int numThreads = strtol(argv[2], NULL, 10);
	
	// Exit if arguments are not positive integers
	if (numIntegers == 0 || numThreads == 0) {
		printf("Usage: ./dotproduct <# of threads>\n");
		exit(0);
	}
	
	// Calculate how many dot products each thread will compute
	int remainder = numIntegers % numThreads;
	int numComputation = (numIntegers - remainder) / numThreads;
	
	// Create two vectors
	int *v1 = (int *) malloc(numIntegers * sizeof(int));
	int *v2 = (int *) malloc(numIntegers * sizeof(int));
	int seqDotProduct = 0;
	
	// Fill vectors with random integers
	for (int i = 0; i < numIntegers; i++) {
		v1[i] = rand();
		v2[i] = rand();
		seqDotProduct += v1[i] * v2[i];
	}
	
	// Print sequential dotproduct and work allocation
	int tempRemainder = remainder;
	printf("Sequential dotproduct: %d\n", seqDotProduct);
	printf("Work allocation across the threads: ");
	for (int i = 0; i < numThreads; i++) {
		if (tempRemainder > 0) {
			printf("%d ", numComputation + 1);
			tempRemainder--;
		}
		else {
			printf("%d ", numComputation);
		}
	}
	printf("\n");
	
	// Create array of pointers to threads
	pthread_t *threadHandler = (pthread_t *) malloc(numThreads * sizeof(threadHandler));
	
	// Create array of pointers to thread argument structures
	Struct **threadArg = (Struct **) malloc(numThreads * sizeof(threadArg));
	
	int arrayPos = 0;
	
	// Create threads
	for (int i = 0; i < numThreads; i++) {
		// Set up argument structure	
		Struct *arg = (Struct *) malloc(sizeof(Struct));
		arg->v1 = v1;
		arg->v2 = v2;
		arg->start = arrayPos;
		arg->numComputation = numComputation;
		arg->dotProduct = 0;
		
		// Distribute remainder computations across threads
		if (remainder > 0) {
			arg->numComputation = arg->numComputation + 1;
			remainder--;
		}
		
		// Update array position for the next thread
		arrayPos += arg->numComputation;
		
		// Place argument structure into handler
		threadArg[i] = arg;
		
		// Place thread into thread handler
		pthread_t thread;
		threadHandler[i] = thread;
		
		// Create thread
		if (pthread_create(&threadHandler[i], NULL, computeDotProduct, (void *) threadArg[i])) {
			fprintf(stderr, "Error while creating thread\n");
			exit(1);
		}
	}
	
	// Wait for all threads to return with their sum
	for (int i = 0; i < numThreads; i++) {	
		void *return_value;
		if (pthread_join(threadHandler[i], &return_value)) {
			fprintf(stderr, "Error while waiting for thread\n");
			exit(1);
		}
	}
	
	// Print multi-threaded dot product
	int multiDotProduct = 0;
	for (int i = 0; i < numThreads; i++) {
		multiDotProduct += threadArg[i]->dotProduct;
	}
	printf("Multi-threaded dotproduct: %d\n", multiDotProduct);
	
	// Free memory
	free(v1);
	free(v2);
	for (int i = 0; i < numThreads; i++) {
		free(threadArg[i]);
	}
	free(threadArg);
	free(threadHandler);
	
	return(0);
}


/* Compute the dot product with the given array of integers. */
void *computeDotProduct(void *thisarg){
	Struct *arg =(Struct *) thisarg;
	for (int i = 0; i < arg->numComputation; i++) {
		arg->dotProduct += arg->v1[arg->start + i] * arg->v2[arg->start + i];
	}
}
