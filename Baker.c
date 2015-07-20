/*
* A program which uses multi-threading to simulate a situation in which a distributor supplies four bakers with 
* items for making a cake. Each baker has an infinite supply of one specific item but requires three other items 
* to make the cake. This is written entire in C.
*
* Created By: Asad Zia.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

/* the macros defining the ingredients required by the bakers */
#define EMPTY 0
#define FLOUR_BUTTER_EGGS 1
#define FLOUR_SUGAR_BUTTER 2
#define BUTTER_SUGAR_EGGS 3
#define FLOUR_SUGAR_EGGS 4

/* the constant defining the ingredients available */
static const char* Supply[] 
{
	"Butter",
	"Flour",
	"Eggs",
	"Sugar"
};

/* the data structure of the shop */
struct shop
{
	pthread_mutex_t mutex;
	pthread_cond_t empty;
	pthread_cond_t full;
	int status;
};

struct shop table;

/* the distributor thread */
void *distributor_thread (void * args)
{
	while (1)
	{
		pthread_mutex_lock (&table.mutex);
		if (table.status != EMPTY)
		{
			pthread_cond_wait(&table.empty, &table.mutex);
		}

		table.status = (int)((double)rand() / RAND_MAX * 4) + 1;

		if (table.status == 1)
		{
			printf("Supplying Flour, Eggs and Butter\n");
		}

		else if (table.status == 2)
		{
			printf("Supplying Flour, Sugar and Butter\n");
		}

		else if (table.status == 3)
		{
			printf("Supplying Butter, Sugar and Egg\n");
		}

		else if (table.status == 4)
		{
			printf("Supplying Sugar, Butter and Eggs\n");
		}
		pthread_cond_broadcast(&table.full);
		pthread_mutex_unlock(&table.mutex);
	}
	return NULL;
}

/* the baker thread function */
void *baker_thread (void* args)
{
	int req = *((int*)args);
	int material;

	while(1)
	{
		pthread_mutex_lock(&table.mutex);

		if (table.status == EMPTY)
		{
			pthread_cond_wait(&table.full, &table.mutex);
		}

		if (req == table.status)
		{
			if (req == 1)
			{
				printf("The Baker with Sugar is collecting the items\n");
				material = 3;
			}

			if (req == 2)
			{
				printf("The Baker with Eggs is collecting the items\n");
				material = 2;
			}

			if (req == 3)
			{
				printf("The Baker with Flour is collecting the items\n");
				material = 1;
			}

			if (req == 4)
			{
				printf("The Baker with Butter is collecting the items\n");
				material = 0;
			}

			table.status = EMPTY;
			printf("Baker with %s is baking the cake\n\n", Supply[material]);
			sleep(1);
			pthread_cond_broadcast(&table.empty);
		}

		pthread_mutex_unlock(&table.mutex);
	}
	return NULL;
}

int main ()
{
	int status;
	pthread_t tid_distributor;
	pthread_t tid_baker_one;
	pthread_t tid_baker_two;
	pthread_t tid_baker_three;
	pthread_t tid_baker_four;

	int one = FLOUR_BUTTER_EGGS, two = FLOUR_SUGAR_BUTTER, three = BUTTER_SUGAR_EGGS, four = FLOUR_SUGAR_EGGS;	
	table.status = EMPTY;

	if (pthread_mutex_init(&table.mutex, NULL) != 0)
	{
		printf("Error in initializing the mutex\n");
		exit(1);
	}

	if (pthread_cond_init(&table.empty, NULL) != 0 && pthread_cond_init(&table.full, NULL) != 0)
	{
		printf("Error in initializing the conditional variables\n");
		exit(1);
	}

	status = pthread_create(&tid_distributor, NULL, distributor_thread, NULL);
	if (status != 0)
	{
		printf("Error in creating the Distributor thread\n");
		exit(1);
	}

	status = pthread_create(&tid_baker_one, NULL, baker_thread, &one);
	if (status != 0)
	{
		printf("Error in creating the Baker thread\n");
		exit(1);
	}

	status = pthread_create(&tid_baker_two, NULL, baker_thread, &two);
	if (status != 0)
	{
		printf("Error in creating the Baker thread\n");
		exit(1);
	}

	status = pthread_create(&tid_baker_three, NULL, baker_thread, &three);
	if (status != 0)
	{
		printf("Error in creating the Baker thread\n");
		exit(1);
	}

	status = pthread_create(&tid_baker_four, NULL, baker_thread, &four);
	if (status != 0)
	{
		printf("Error in creating the Baker thread\n");
		exit(1);
	}

	pthread_join(tid_baker_one, NULL);
	pthread_join(tid_baker_two, NULL);
	pthread_join(tid_baker_three, NULL);
	pthread_join(tid_baker_four, NULL);
	pthread_join(tid_distributor, NULL);

	if (pthread_mutex_destroy(&table.mutex) != 0)
	{
		printf("Error in destorying the mutex\n");
		exit(1);
	}

	if (pthread_cond_destroy(&table.full) != 0 && pthread_cond_destroy(&table.empty) != 0)
	{
		printf("Error in destroying the conditional variables\n");
		exit(1);
	}

	pthread_exit(NULL);
	return 0;
}
