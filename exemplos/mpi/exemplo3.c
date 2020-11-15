#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv){

	int id, np,i,n,soma;
	
	char str[100];
	MPI_Status s;
	
	MPI_Init(&argc, &argv);
	
	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &np);

	n = id*2;
	soma = n;
	for(i =1; i<np; i++)
	{			
		MPI_Send(&n, 1, MPI_INT, (id+1)%np, 100, MPI_COMM_WORLD);
		MPI_Recv(&n, 1, MPI_INT, (id-1+np)%np, 100, MPI_COMM_WORLD,&s);
		soma+=n;
	}
	
	printf("ID: %d Soma: %d\n",id, soma);

	MPI_Finalize();	
}
