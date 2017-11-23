#include <stdio.h>
#include <errno.h>

const int maxResources = 5;
int resources = maxResources;

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
	while(true){
		pthread_mutex_lock(  &mtx);
		if( decrease_count( count ) == -1 )
			continue;
		printf("Got %d resources %d remaining\n",count,resources);
		int i = 0 ;
		for( i = 0; i < count;++i)  ;
		pthread_mutex_unlock(  &mtx);
		
	}	
}

int main(int argc, char *argv[]){
	
	if( pthread_mutex_init( &mtx, NULL){
		perror(NULL);
		return errno;
	}

	int v[] = {3,2,3,2,4,1,3,4)
	int n = sizeof(v)/sizeof(n);
	
	int i;
	for(i=0;i<n;++i){
		
	}
	
		
	
	
	return 0;
}
