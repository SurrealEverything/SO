#include <stdio.h>
#include <unistd.h>
#include <sys/types.h> 
#include <stdlib.h>  
#include <errno.h> 
#include <linux/sched.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

char buffer[256];
void writeFromString(int n){
	write(1, buffer , n );
}

int main(int argc, char *argv[])
{	
	writeFromString( sprintf(buffer, "\nStarted parent %d\n",getpid()) );
	if(argc == 1)
		return -1;
	int i;
	for(i=1; i < argc; ++i)
	{	
		pid_t  pid = fork();
		if( pid == -1)
			return -1;
		else if( pid == 0){
			int n = atoi(argv[i]);
			writeFromString( sprintf(buffer, "%d: %d",n,n) );
			while( n != 1){
				if(n %2 == 0)
					n = n/2;
				else
					n = 3*n +1;
				writeFromString( sprintf(buffer, " %d",n) );
			}
		
			
			writeFromString( sprintf(buffer, "\nDone Parent %d Me %d\n",getppid(),getpid()) );
			return 0;
		}
		else{
			//writeFromString( sprintf(buffer , "My PID=%d , Child PID=%d\n",getpid() , pid) );
		}	
	}

	return 0;
}

