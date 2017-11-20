#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

void* reverse(void* voidSource){
	if( voidSource == 0){
		printf("Error: Null pointer received as argument in the created Thread");
		perror(NULL);
		
	}	
	char *source = (char*)voidSource;	
	
	int stringLength = strlen(source);
	char *result = malloc( stringLength + 1);

	int index;
	for(index = 0; index < stringLength; ++index)
		result[index] = source[stringLength - index - 1] ; 
	result[stringLength ] = '\0';
	pthread_exit((void*)result);
	
} 

int main(int argc , char *argv[])
{
	
	if(argc != 2){
		printf("Error: Invalid number of arguments : %d\n",argc - 1);
		return errno;
	}

	
	int stringLength = strlen(argv[1]);
	if( stringLength == 1){
		printf("%s\n",argv[1]);
		return 0;	
	} 
	
	pthread_t thr;
	if( pthread_create( &thr, NULL, reverse , argv[1] ) ){
		perror(NULL);
		printf("Error: Failed to create a new Thread\n");
		return errno;
	}
	
	void *voidResult;
	if( pthread_join(thr, &voidResult) ){
		perror(NULL);
		printf("Error: Failed to join the created Thread\n");
		return errno;
	}
	
	if( voidResult == 0 ){
		printf("Error: Null pointer returned from the created Thread");
		perror(NULL);
		return errno;
	}
	char *charResult = (char*)voidResult;
	printf("%s\n", charResult);
	free(voidResult);

	return 0;
}
