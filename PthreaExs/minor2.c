/* minor2.c - using producer-consumer paradigm. */
//Giovanni Rebosio gdr0051 10849933

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NITEMS 10		// number of items in shared buffer
typedef pthread_cond_t Cond;

void cond_wait(Cond *cond, pthread_mutex_t *mutex1)
{
	int n = pthread_cond_wait(cond, mutex1);
	if( n != 0)
	{
		printf("cond_wait failed\n");
	}
}

void cond_signal(Cond *cond)
{
	int n = pthread_cond_signal(cond);
	if(n != 0)
	{
		printf("cond_signal failed\n");
	}
}

// shared variables
char shared_buffer[NITEMS];	// echo buffer
int shared_count = 0;		// item count
int done = 1;			//spurious wakeup condition

pthread_mutex_t mutex;		// pthread mutex

//initialize conditions
Cond condFull;
Cond condEmpty;
unsigned int prod_index = 0; 	// producer index into shared buffer
unsigned int cons_index = 0; 	// consumer index into shard buffer

// function prototypes
void * producer(void *arg);
void * consumer(void *arg);

int main() 
{ 
	pthread_t prod_tid, cons_tid1, cons_tid2; 
	
	// initialize pthread variables
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&condFull, NULL);
	pthread_cond_init(&condEmpty, NULL);
	
	// start producer thread
	pthread_create(&prod_tid, NULL, producer, NULL);

	// start consumer threads
	pthread_create(&cons_tid1, NULL, consumer, NULL);
	pthread_create(&cons_tid2, NULL, consumer, NULL);
	
	// wait for threads to finish
	pthread_join(prod_tid, NULL);
	pthread_join(cons_tid1, NULL);
	pthread_join(cons_tid2, NULL);
			
	// clean up
	pthread_mutex_destroy(&mutex);
	
	return 0;
}


// producer thread executes this function
void * producer(void *arg)
{
	char key;

	//lock mutex
	pthread_mutex_lock(&mutex);

	printf("Enter text for producer to read and consumer to print, use Ctrl-C to exit.\n\n");

	// this loop has the producer read data in from stdin and place the read data on the shared buffer
	while (1)
	{
		// read input key
		scanf("%c", &key);
		
		//While there is something in count, signal consumer and wait
		while(shared_count > 0 || done == 0)
		{
			cond_signal(&condEmpty);
			cond_wait(&condFull, &mutex);
		}
		
		done = 0;
		// store key in shared buffer
		shared_buffer[prod_index] = key;

		// update shared count variable
		shared_count++;

		// update producer index
		if (prod_index == NITEMS - 1)
			prod_index = 0;
		else
			prod_index++;
		done = 1;
	}

	//unlock mutex
	pthread_mutex_unlock(&mutex); 
	return NULL;
}

// consumer thread executes this function
void * consumer(void *arg)
{
	
	char key;

	//lock mutex
	pthread_mutex_lock(&mutex);
	long unsigned int id = (long unsigned int)pthread_self();

	// this loop has the consumer get data from the shared buffer and print to stdout
	while (1)
	{
		//While there is nothing in count, signal producer and wait
		while(shared_count == 0 || done == 0)
		{
			cond_signal(&condFull);
			cond_wait(&condEmpty, &mutex);
		}
		
		done = 0;
		// read key from shared buffer
		key = shared_buffer[cons_index];
		
		// echo key
		printf("consumer %lu: %c\n", (long unsigned int) id, key);

		// update shared count variable
		shared_count--;

		// update consumer index
		if (cons_index == NITEMS - 1)
			cons_index = 0;
		else
			cons_index++;	
		done = 1;
	}

	//unlock mutex
	pthread_mutex_unlock(&mutex);
	return NULL;
}
