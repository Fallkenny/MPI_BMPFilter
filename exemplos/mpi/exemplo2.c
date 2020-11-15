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
	
	srand(id);
	n = rand()%10;
	
		
	if(id != 0)
	{

		MPI_Send(&n, 1, MPI_INT, 0, 100, MPI_COMM_WORLD);
	}
	else
	{
		soma = n;
		for(i =1; i<np; i++)
		{
			MPI_Recv(&n,1,MPI_INT, i, 100, MPI_COMM_WORLD, &s);
			soma +=n;
		}
		printf("Soma: %d\n", soma);
	}
	
	if(id == 0)
	{
		for(i =1; i<np; i++)
		{
			MPI_Send(&soma, 1, MPI_INT, 0, 200, MPI_COMM_WORLD);
		}
		
	}
	else
	{
		MPI_Recv(&soma,1,MPI_INT, i, 200, MPI_COMM_WORLD, &s);
	}
	printf("ID: %d Soma: %d\n",id, soma);

	MPI_Finalize();	
}
