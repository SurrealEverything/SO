#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h> 
#define THREAD_COUNT 5
pthread_mutex_t mtx;
sem_t sem;
int reachedBarrierCount;
int barrier_release()
{
    int i;
    for(i=0; i<THREAD_COUNT-1; ++i)//pornesc semaforul pt threadurile care asteapta
    {
        //pthread_mutex_lock(&mtx);
        //reachedBarrierCount--;
        //pthread_mutex_unlock(&mtx);
        if( sem_post (&sem ) ) 
        {
            perror (NULL ) ;
            return errno ;
        }
    }
}
int barrier_point()
{
    pthread_mutex_lock(&mtx);
    reachedBarrierCount++;//contorizez firele ajunse la bariera
    pthread_mutex_unlock(&mtx);
    if(reachedBarrierCount==THREAD_COUNT)//daca toate(in afara de ultimul) threadurile asteapta la bariera
        barrier_release();//pornesc semaforul pt threadurile care asteapta
    else if( sem_wait (&sem) ) //astept la semafor
    {
        perror (NULL ) ;
        return errno ;
    }
    return 0;
}
void *tfun ( void *v )
{
    int *tid = ( int * ) v ;
    printf ("%d reached the barrier \n" , *tid ) ;
    barrier_point ();
    printf ("%d passed the barrier \n" , *tid );
    free ( tid );
    return NULL;
}
int main()
{
    printf("NTHRS=%d\n", THREAD_COUNT);
    
    if ( sem_init (&sem , 0 , 0 ) )//init sem
    {
        perror (NULL ) ;
        return errno ;
    }
    if( pthread_mutex_init (&mtx , NULL) )//init mutex
    {
        perror (NULL ) ;
        return errno ;
    }

    pthread_t thr[THREAD_COUNT];
    int i;

    int *tid;
    for(i=0; i<THREAD_COUNT; ++i)//creez threaduri
    {
        tid=(int *)malloc(sizeof(int));
        *tid=i;
        if(pthread_create(&thr[i], NULL, tfun, (void *)tid))
        {
            perror(NULL);
            return errno ;
        }
    }

    for(i=0; i<THREAD_COUNT; ++i)//astept terminarea threadurilor
    {
        if(pthread_join(thr[i], NULL)) 
        {
            perror(NULL);
            return errno ;
        }
    }
}
