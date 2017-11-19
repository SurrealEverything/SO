#include <stdio.h>
#include <string.h>
#include <unistd.h>


char buffer[256];
void writeFromString(int n){
	write(1, buffer , n );
}

int main(int argc, char *argv[])
{
	pid_t  pid = fork();
	if( pid == -1)
		return -1;
	else if( pid == 0){
		char *envp[] = { NULL };
 		char *argv[] = { "/bin/ls", "-a", NULL };
		writeFromString( sprintf(buffer, "\nChild %d started!\n",getpid()) );
		execve ( "/bin/ls" , argv , envp ) ;
		perror(NULL) ;
		writeFromString( sprintf(buffer, "\nChild %d finished!\n",getpid()) );
		return 0;
	}
	else{
		writeFromString( sprintf(buffer , "My PID=%d , Child PID=%d\n",getpid() , pid) );
	}	
	
	return 0;
}

