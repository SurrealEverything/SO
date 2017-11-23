#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>


struct _matrix{
	int n,m;
	int **holder;
};
struct _matrix A,B,C;

void deallocateMatrix(int **x,int n,int m){
	int i;
	for(i = 0; i < n; ++i)
		free( x[i] );
	free(x);
} 


void deleteMatrix (struct  _matrix *X){
	if( X->holder != 0 )
		deallocateMatrix(X->holder,X->n,X->m); 

	X->n = 0;
	X->m = 0;
}

int** allocate_matrix(const int n,const int m){
	int **x = (int**)malloc( n * sizeof(int*) );
	int i = 0;
	for(i = 0; i< n; ++i)
		x[i] = (int*)malloc( m * sizeof(int) );
	return x;
	
}

struct _matrix create_matrix(FILE *input){
	struct _matrix X;
	fscanf(input,"%d %d",&X.n,&X.m);
	X.holder = (int**)allocate_matrix(X.n,X.m);
	return X;
}

void readMatrix(struct _matrix *X,FILE *input){
	int i,j;
	for(i=0;i < X->n ;++i)
		for(j=0;j < X->m ;++j)
			fscanf(input,"%d", &X->holder[i][j]);
}

void printMatrix(struct _matrix X){
	int i,j;
	for(i=0;i < X.n ;++i){
		for(j=0;j < X.m ;++j)
			printf("%d ", X.holder[i][j]);
		printf("\n");
	}

}

void read_matrix(void* voidSource){
	printf("Input file is \"%s\"\n",(char*)voidSource);
	FILE *fin = fopen(  (char*)voidSource , "r");
	A = create_matrix(fin);	
	readMatrix(&A,fin);
	printf("SUCCES: Finished reading matrix A from *%s* \n",(char*)voidSource);
	if(A.n <= 10 && A.m <= 10 )
		printMatrix(A);


	B = create_matrix(fin);	
	readMatrix(&B,fin);
	printf("SUCCES: Finished reading matrix B from *%s* \n",(char*)voidSource);
	if(B.n <= 10 && B.m <= 10 )
		printMatrix(B);
	
	
}
void* sum(void* arg){
	int i = ((int*)arg)[0], j = ((int*)arg)[1] , n = ((int*)arg)[2];
	int k = 0;
	int suma = 0;
	for(k=0;k<n;++k)
		suma += A.holder[i][k] * B.holder[k][j];
	
	//C.holder[i][j] = suma;
	//return suma;
}

void* multiplicate_matrix(void *useless){
	if( A.m != B.n || A.holder == 0 || B.holder == 0 )
		pthread_exit( (void*)0 );
	int i,j;
	struct _matrix *res;
	C.holder = (int**)allocate_matrix(A.n,B.m);
	C.n = A.n;	
	C.m = B.m;	
	printf("Started calculating C = A * B \n");

	pthread_t **thr = (pthread_t**)allocate_matrix(A.n,B.m);
	
		
	
	for(i=0;i<A.n;++i){
		for(j=0;j<B.m;++j){
			//C.holder[i][j] = sum(i,j,A.m);	
			int arg[] = {i,j,A.m};
			if( pthread_create( &thr[i][j], NULL, sum , (void*)arg ) ){
				perror(NULL);
				printf("Error: Failed to create a new Thread\n");
				return errno;
			}
		}
	}

	for(i=0;i<A.n;++i){
		for(j=0;j<B.m;++j){
			pthread_join(thr[i][j] , (void*)(&C.holder[i][j]) );
			
		}
	}
		
	
	res = (struct _matrix*)malloc( sizeof(struct _matrix) );
	*res = C;
	printf("SUCCES: Finished C = A * B \n");
	pthread_exit( (void*)res);
}
 

int main(int argc , char *argv[])
{
	
	read_matrix(argv[1]);	
	
	
	
	pthread_t thr;
	if( pthread_create( &thr, NULL, multiplicate_matrix , NULL ) ){
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
	
	if( voidResult == 0 || voidResult == (void*)(-1) ){
		printf("Error: Null pointer returned from the created Thread");
		perror(NULL);
		return errno;
	}
	struct _matrix *result = (struct _matrix*)voidResult;
	printMatrix(*result);
	
	deleteMatrix(&A);
	deleteMatrix(&B);
	deleteMatrix(result);

	return 0;
}
