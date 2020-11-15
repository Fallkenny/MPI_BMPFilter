#include <mpi.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv){

	int id, nproc,i;
	char str[100];
	MPI_Status s;
	
	MPI_Init(&argc, &argv);
	
	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &nproc);
	
	
	if(id == 0)
	{
		strcpy(str, "Alo mundo");
		for(i=1;i<nproc;i++)
			MPI_Send(str, strlen(str)+1, MPI_CHAR, i, 100, MPI_COMM_WORLD);
	}
	else
	{
		MPI_Recv(str, 100, MPI_CHAR, 0, 100, MPI_COMM_WORLD, &s);
	}
	
	
	printf("ID: %d NPROC: %d String: %s\n", id, nproc, str);

	MPI_Finalize();	
}

