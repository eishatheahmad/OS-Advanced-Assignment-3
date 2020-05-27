#include<string.h>
#include <pthread.h> 
#include <semaphore.h> 
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h> 
#include <sys/shm.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include<sys/syscall.h>
#include <sys/ipc.h>
#include <sys/ioctl.h>
#include <sys/types.h>



int potentialCPatients=0;
int coronaPCount=0;
int fluPCount=0;

sem_t coronaPatient; 
sem_t fluPatient;


struct s_threadId {
  pthread_mutex_t   mtx;    /* mutex & condition to allow main thread to
                               wait for the new thread to  set its TID */
  pthread_cond_t    cond;   /* '' */
  pid_t             id;     /* to hold new thread's TID */
  int               ready;  /* to indicate when 'id' is valid, in case
                               the condition wait gets interrupted */
};



void *fluProcess(void *flu)
{
 
	while(true)
	{

		sem_wait(&coronaPatient);
		fluPCount++;

		struct s_threadId  *temp=flu;
		pthread_mutex_lock(&temp->mtx);
		temp->id=syscall(SYS_gettid);

		//pthread_mutex_lock(&mutex);
		printf("It is a Flu patient\n");
		potentialCPatients--;
		//recovery=true;
		temp->ready=1;

		pthread_cond_signal(&temp->cond);

		pthread_mutex_unlock(&temp->mtx);

		sem_post(&fluPatient);

		return NULL;


	}
}


void *coronaProcess(void *corona)
{

	struct s_threadId  *temp=corona; 
	//pthread_mutex_lock(&mutex);
	coronaPCount++;
	pthread_mutex_lock(&temp->mtx);
	//temp->id=0;
	temp->id=syscall(SYS_gettid);



	while(true)
	{
		sem_wait(&fluPatient);

		int rnd= rand()%2;

		if(rnd==0)
		{
			printf("Test1: It is a corona Patient\n");
			printf("The patient is Tested Again For Corona\n");

			rnd= rand()%2;

			if(rnd==0)
			{
				printf("Test2: It is a corona Patient\n");
				printf("The patient is Tested Again For Corona\n");

			}
			else
			{
				printf("Test2: Corona Test Negative\n");
				printf("The patient is recovered\n");


			}

		}
		else
		{
			printf("Test1: Corona Test Negative\n");
			printf("The patient is Tested Again For Corona\n");

			rnd= rand()%2;

			if(rnd==0)
			{
				printf("Test2: It is a corona Patient\n");
				printf("The patient is Tested Again For Corona\n");

			}
			else
			{
				printf("Test2: Corona Test Negative\n");
				printf("The patient is recovered\n");


			}


		}

		if(rnd==0)
		{
			printf("The patient is not recovered\n");

		}

		potentialCPatients--;
		//recovery=true;
		temp->ready=1;
		pthread_cond_signal(&temp->cond);
		pthread_mutex_unlock(&temp->mtx);
		sem_post(&coronaPatient);
		return NULL;

	}



}




int main()
{

	int count=0;
	int n=0;

	printf("Enter number of paients\n");
	scanf("%d",&n);


	potentialCPatients=n;

	sem_init(&coronaPatient,0,n);
	sem_init(&fluPatient,0,n);
	pthread_t thread;

	struct s_threadId TID;
	pthread_cond_init(&TID.cond, NULL);  
	pthread_mutex_init(&TID.mtx, NULL);



	int i=0;

	for(i=0;i<n;i++)
	{

		int r= rand()%2;
		if(r==0) //flu
		{
			pthread_mutex_lock(&TID.mtx);
			//pthread_create(&pro,NULL,&writer,NULL);
			TID.ready=0;
			pthread_create(&thread,NULL,&fluProcess,&TID);

		}
		else if(r==1)
		{
			pthread_mutex_lock(&TID.mtx);
			//pthread_create(&pro,NULL,&writer,NULL);
			TID.ready=0;
			pthread_create(&thread,NULL,&coronaProcess,&TID);

		}
		else
		{
		/* Wait on the condition until the ready flag is set */
			while(!TID.ready)
			{

			pthread_cond_wait(&TID.cond,&TID.mtx);

			}


		}

		printf("\npatient id =%d\n",TID.id);
		//pthread_mutex_unlock(&mutex);
		pthread_mutex_unlock(&TID.mtx);
		pthread_join(thread, NULL);
		//pthread_join(threadC, NULL);


	}

	/* When we're completely done with the struct we need to clean up the mutex and condition variable */
	pthread_mutex_destroy(&TID.mtx);
	pthread_cond_destroy(&TID.cond);

	printf("\ncoronaPatients =%d\n",coronaPCount);
	printf("\nfluPatients =%d\n",fluPCount);


}
