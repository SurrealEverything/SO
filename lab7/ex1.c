#include <stdio.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>

const int maxResources = 5;
int resources = 5;

int decrease_count(int count){
	if( resources < count )
		return -1;
	else
		resources -= count;
	return 0; 	
}

int increase_count(int count){
	resources += count;
	return 0; 	
}


pthread_mutex_t mtx;

void* tryToReserve(void *arg){
	int count = *((int*)arg);
	if( count > maxResources ){
		printf("Trying to allocate more resources(%d) than total\n",count);
		fflush(stdout);
		return 0;
	}
	while( 1 ){
		pthread_mutex_lock(  &mtx);
		if( decrease_count( count ) == -1 ){
			pthread_mutex_unlock(  &mtx);
			continue;
		}
		printf("Got %d resources %d remaining\n",count,resources);
		fflush(stdout);
		int i = 0 ;
		for( i = 0; i <  count;++i)  ;
		increase_count( count);
		printf("Released %d resources %d remaining\n",count,resources);
		fflush(stdout);
		
		break;
		
	}
	
	pthread_mutex_unlock(  &mtx);
}

int main(int argc, char *argv[]){
	
	if( pthread_mutex_init( &mtx, NULL) ){
		perror(NULL);
		return errno;
	}

	int v[] = {3,2,3,2,5,4,1,3,4};
	int n = sizeof(v)/sizeof(int);
	
	int i;
	pthread_t *thm = (pthread_t*)malloc(n*sizeof(pthread_t) ) ;
	for(i=0;i<n;++i){
		pthread_create( &thm[i] , NULL , tryToReserve , &v[i]);
	}
	
	for(int i=0;i<n;++i){
		pthread_join(thm[i] , NULL) ; 
	}
		
	
	
	return 0;
}
