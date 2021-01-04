#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <math.h>
#define b 20
int counter=0;
int buffer[b];
sem_t csem , n , empty , bsem;

void * mcounter ( void * arg){
int *x =arg;
sleep(rand()%9); // generate random number between 0 and 9 sec
printf("Counter thread %d : received a message \n",*x);
printf("Counter thread %d : waiting to wrtie \n",*x);
sem_wait(&csem);
counter++;
printf("Counter thread %d : now adding to counter , counter value= %d \n",*x,counter);
sem_post(&csem);

}

void * mmonitor ( void * arg){
int *t1 = arg;
int value ;
int index = 0 ;
int  isbufferfull;
while(1){
sleep(*t1);
printf("Monitor thread : waiting to read the counter \n");
sem_wait(&csem);
value=counter;
counter=0;
printf("Monitor thread : reading a count of value = %d \n",value);
sem_post(&csem);


sem_getvalue(&empty,&isbufferfull);
// get the number of empty spaces in buffer
if(isbufferfull<=0)
printf("Monitor thread : buffer full!!! \n");

sem_wait(&empty);
sem_wait(&bsem);
buffer[index]=value;
printf("Monitor thread : writing to buffer at position %d \n",index);
index ++ ;
if(index==b)
index=0;

sem_post(&bsem);
sem_post(&n);
    }

}

void * mcollector (void * arg){
int index=0;
int isbufferempty;
while(1){
sleep(rand() % 9 );

sem_getvalue(&n,&isbufferempty);
if(isbufferempty==0){
printf("Collector thread : nothing is in the buffer!!!!\n");
}
sem_wait(&n);
sem_wait(&bsem);

printf("Collector thread : reading from the buffer at position %d \n", index);
index++;
if(index==b)
{
index=0;
}
sem_post(&bsem);
sem_post(&empty);




    }

}


void main()
{

int N=10, t1 = 1 ;

sem_init(&csem,0,1);// counter semaphore for mutex
sem_init(&bsem,0,1);// buffer semaphore for mutex
sem_init(&n,0,0);  // how many items in buffer
sem_init(&empty,0,b);
//// inilizing the semaphore first 0 for being between threads of a process and 1 is the size of it

pthread_t mcounters[N];
pthread_t mmonitors, mmcollectors;

///////////////////////////////// lunching the mcounter threads ////////////////////////////
int i ;
int array[N];
for(i=0 ; i<N ; i++){
array[i]=i;
pthread_create(&mcounters[i],NULL,mcounter,&array[i]);
}

pthread_create(&mmonitors,NULL,mmonitor,&t1);
pthread_create(&mmcollectors,NULL,mcollector,NULL);

for (i=0 ; i <N ;i++)
pthread_join(mcounters[i],NULL);

pthread_join(&mmonitors,NULL);
pthread_join(&mmcollectors,NULL);
pthread_join(&mmonitors,NULL);
printf("out");
}