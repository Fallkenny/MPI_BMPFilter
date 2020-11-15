#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
/*-----------------------------------------------------*/
int main(int argc, char **argv){
	double termo, p = 0;
	long i, ntermos;
	int id, np;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &np);

	if(argc !=2)
	{
		if(id==0)
		{
			printf("%s <num_termos>", argv[0]);
		}
		MPI_Finalize();
		exit(0);
	}
	
	ntermos = atol(argv[1]);


	for( i=id+1; i<=ntermos; i+=np){
		termo = 1.0 / (2*i-1);
		if ( i & 1 ){ 		
			p += termo;
		}
		else{
			p -= termo;
		} 
	}

	p *= 4;

	if(id !=0){
		MPI_Send(&p, 1, MPI_DOUBLE, 0, 100, MPI_COMM_WORLD);
	}
	else{
		MPI_Status s;
		double total = p;
		for(i=1; i<np; i++){
			MPI_Recv(&p,1, MPI_DOUBLE, i, 100, MPI_COMM_WORLD, &s);
			total+=p;
		}
		printf("PI: %.15f\n", total);
		
	}
	
///	printf("id: %d PI: %.15f\n", id, p);
	MPI_Finalize();
}
/*-----------------------------------------------------*/

