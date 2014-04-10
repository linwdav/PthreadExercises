/**
 * Giving out bathroom keys.
 * @author David Lin
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>


/* Thread argument structure. */
typedef struct Struct 
{
	int id;
	int *numKeys;
	pthread_mutex_t *key;
	pthread_cond_t *cond;
} Struct;


/* Function declaration. */
void *doWork(void *arg);

int getRandom();


/* Execute program. */
int main(int argc, char *argv[]) 
        {
	// Check that there are two command-line arguments
	
	if (argc < 3) 
	{
		printf("Usage: ./coffeeshop <# of threads> <seed time> \n");
		exit(0);
	}
	
	// Convert arguments to integers
	int numThreads = strtol(argv[1], NULL, 10);
	int seedTime = strtol(argv[2], NULL, 10);
	
	// Exit if arguments are not positive integers
	if (numThreads == 0 || seedTime == 0) {
		printf("Usage: ./coffeeshop <# of threads> <seed time> \n");
		exit(0);
	}
	
	// Seed
	srand(seedTime);
	
	// Create array of pointers to threads
	pthread_t *threadHandler = (pthread_t *) malloc(numThreads * sizeof(threadHandler));
	
	// Create array of pointers to thread argument structures
	Struct **threadArg = (Struct **) malloc(numThreads * sizeof(threadArg));
	
	// Create key
	pthread_mutex_t *key = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
	if (pthread_mutex_init(key, NULL)) {
		fprintf(stderr, "Error while creating lock\n");
	}
	
	// Create key counter
	int *numKeys = (int *) malloc(sizeof(int));
	*numKeys = 2;
	
	// Create condition
	pthread_cond_t *cond = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
	if (pthread_cond_init(cond, NULL)) {
		fprintf(stderr, "Error while creating condition variable\n");
	}
	
	// Create threads
	for (int i = 0; i < numThreads; i++) {
		// Set up argument structure	
		Struct *arg = (Struct *) malloc(sizeof(Struct));
		arg->id = i;
		arg->key = key;
		arg->numKeys = numKeys;
		arg->cond = cond;
		
		// Place argument structure into handler
		threadArg[i] = arg;
		
		// Place thread into thread handler
		pthread_t thread;
		threadHandler[i] = thread;
		
		// Create thread
		if (pthread_create(&threadHandler[i], NULL, doWork, (void *) threadArg[i])) {
			fprintf(stderr, "Error while creating thread\n");
			exit(1);
		}
	}
	
	// Wait for all threads to return
	for (int i = 0; i < numThreads; i++) {	
		void *return_value;
		if (pthread_join(threadHandler[i], &return_value)) {
			fprintf(stderr, "Error while waiting for thread\n");
			exit(1);
		}
	}
	
	// Free memory
	for (int i = 0; i < numThreads; i++) {
		free(threadArg[i]);
	}
	free(threadArg);
	free(threadHandler);
	free(cond);
	free(key);
	free(numKeys);
	
	return(0);
}

void *doWork(void *thisarg) {
	Struct *arg = (Struct *) thisarg;
	// Enter coffee shop
	printf("Thread %d enters the coffee shop\n", arg->id);
	usleep(2000000);
	
	for (int i = 0; i < 10; i++) {
		// Drink coffee
		printf("Thread %d is drinking coffee\n", arg->id);
		usleep(getRandom());
		
		// Get key and use bathroom
		pthread_mutex_lock(arg->key);
		while (*(arg->numKeys) == 0) {
			printf("Thread %d is waiting for a key\n", arg->id);
			pthread_cond_wait(arg->cond, arg->key);
		}
		*(arg->numKeys) -= 1;
		printf("Thread %d got a key\n", arg->id);
		printf("Thread %d is using the bathroom\n", arg->id);
		pthread_mutex_unlock(arg->key);
		
		// It takes time to use the bathroom
		usleep(getRandom());
		
		// Return key
		pthread_mutex_lock(arg->key);
		*(arg->numKeys) += 1;
		printf("Thread %d put a key back on the board\n", arg->id);
		pthread_mutex_unlock(arg->key);
		pthread_cond_broadcast(arg->cond);
	}
	
	// Leave the coffee shop
	printf("Thread %d leaves the coffee shop\n", arg->id);
}

/* Returns a random integer between 2,000,000 and 10,000,000. */
int getRandom() {
	return (2000000 + (rand() % 8000000));
}
