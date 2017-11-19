#include <stdio.h>
#include <unistd.h>
#include <sys/types.h> 
#include <stdlib.h>  
#include <errno.h> 
#include <linux/sched.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h> /* For mode constants */
#include <fcntl.h> /* For O_* constants */ 

char buffer[256];
void writeFromString(int n){
	write(1, buffer , n );
}


int defaultAllocationSizePerElement ; //= sysconf(_SC_PAGE_SIZE);
const int maxAllowedInt = 100000;
const int maxArgumentCount  = 30;


void waitForAll(int argc){
	int i;
	for(i=1; i < argc; ++i)
		wait(NULL);

}

int main(int argc, char *argv[])
{	
	defaultAllocationSizePerElement = sysconf(_SC_PAGE_SIZE);

	writeFromString( sprintf(buffer, "\nStarted parent %d\n",getpid()) );
	if(argc == 1)
		return -1;
	if( argc > (maxArgumentCount+1) )
		return -1;


	char shm_name[] = "myshm";
	int shm_fd = shm_open( shm_name,  O_CREAT | O_RDWR, S_IRUSR | S_IWUSR); 	
	if( shm_fd < 0 ){
		perror(NULL);
		return errno;
	}	


	size_t shm_size = 4 * argc * defaultAllocationSizePerElement;
	if( ftruncate(shm_fd, shm_size) == -1){
		perror(NULL);
		shm_unlink(shm_name);
		return errno;
	} 

	

	int i;

	int offset = 0;
	for(i=1; i < argc; ++i)
	{	
		
		void *shm_ptr = mmap( 0, defaultAllocationSizePerElement , PROT_WRITE|PROT_READ , MAP_SHARED, shm_fd, offset );
		if( shm_ptr == MAP_FAILED ){
			writeFromString( sprintf(buffer, "Failed to mmap %d -> %d \n",getppid(),getpid()) );
			perror(NULL);
			//shm_unlink(shm_name);
			return errno;
		}		


		int *shm_ptr_int = (int*)shm_ptr;
		pid_t  pid = fork();
		if( pid == -1)
			return -1;
		else if( pid == 0){


			int n = atoi(argv[i]);
			if(n > maxAllowedInt){
				writeFromString( sprintf(buffer, "\Error. Max input value is %d\n",maxAllowedInt) );
				return -1;
			}


			int index = 0;
			while( n != 1){
				shm_ptr_int[index++] = n;
				if(n %2 == 0)		n = n/2;	
				else			n = 3*n +1;
			}
			shm_ptr_int[ index++] = 1;


			if( munmap(shm_ptr, defaultAllocationSizePerElement) == -1){
				writeFromString( sprintf(buffer, "Failed to unmap, Parent %d Me %d\n",getppid(),getpid()) );
				return -1;
			}
			
		
			writeFromString( sprintf(buffer, "Done %d -> %d\n",getppid(),getpid()) );
			return 0;

		}

		offset += (defaultAllocationSizePerElement);	
		
			
		
	}


	waitForAll(argc);

	
	void *shm_ptr = mmap( 0, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0 );
	if( shm_ptr == MAP_FAILED ){
		writeFromString( sprintf(buffer, "Failed to unmap while writting results, ME(parent) %d \n",getpid()) );
		perror(NULL);
		shm_unlink(shm_name);
		return errno;
	}
	

	int *shm_ptr_int = (int*)shm_ptr;
	offset = 0;
	for(i = 1; i < argc; ++i){
		int j = offset;
		writeFromString( sprintf(buffer, "%d:",shm_ptr_int[j] ));
		while(  shm_ptr_int[j] != 1 && shm_ptr_int[j] != 0 ){
			writeFromString( sprintf(buffer, " %d",shm_ptr_int[j]   ) );
			++j;
		}
		writeFromString( sprintf(buffer, " 1\n"  ) );
		offset +=  defaultAllocationSizePerElement/sizeof(int);
	}		
	
	
	writeFromString( sprintf(buffer, "\nDone Parent %d \n",getpid()) );
	shm_unlink(shm_name);

	return 0;
}

