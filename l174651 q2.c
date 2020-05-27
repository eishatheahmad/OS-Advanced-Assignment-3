#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

sem_t sem1, sem2, sem3, sem4;
char *buff1; char *buff2;
FILE *fp1, *fp2;

//reading from file1
void *processA(void *ptrA)
{
	sem_wait(&sem1);

	if((fp1=fopen("file-1.txt","r"))==NULL)
	{
		printf("Error! Cannot open File1");
		exit(1);

	}
	else
	{
		fgets(buff1,11,(FILE*)fp1);
		fclose(fp1);
		sem_post(&sem2);


	}

}

//reading from file2
void *processB(void *ptrB)
{
	sem_wait(&sem2);

	if((fp2=fopen("file-2.txt","r"))==NULL)
	{

		printf("Error! Cannot open File2");
		exit(1);

	}
	else
	{

		char temp[10];
		fgets(temp,11,(FILE*)fp2);

		int j=10;
		int i=0;
		for(i=0; i<20;i++)
		{
			buff1[j]=temp[i];
			j++;
		}
		buff1[j++]='\0';

		fclose(fp2);
		sem_post(&sem3);


	}

}

//copying from buff1 to buff2
void *processC(void *ptrC)
{

	sem_wait(&sem3);

	strcpy(buff2,buff1);

	sem_post(&sem4);


}

//printing buff2
void *processD(void *ptrD)
{
	sem_wait(&sem4);

	printf("\n");
	int i=0;
	printf("\nFirst file:");
	for(i=0; i<20;i++)
	{

		if(i<10)
		{
			printf("%c",buff2[i]);
		}
		else if(i==10)
		{

			printf("\nSecond file:");
			printf("%c",buff2[i]);

		}
		else
		{
			printf("%c",buff2[i]);
		}


	}
	printf("\n");


}

int main()
{
//dynamically allocating memory
	buff1= (char*) malloc(21*sizeof(char));
	buff2= (char*) malloc(21*sizeof(char));


	pthread_t id1, id2, id3,id4;

	sem_init(&sem1, 0, 1);
	sem_init(&sem2, 0, 0);
	sem_init(&sem3, 0, 0);
	sem_init(&sem4, 0, 0);




	pthread_create(&id1, NULL, &processA, NULL);
	pthread_create(&id2, NULL, &processB, NULL);
	pthread_create(&id3, NULL, &processC, NULL);
	pthread_create(&id4, NULL, &processD, NULL);


	pthread_join(id1, NULL);
	pthread_join(id2, NULL);
	pthread_join(id3, NULL);
	pthread_join(id4, NULL);


return 0;
}
