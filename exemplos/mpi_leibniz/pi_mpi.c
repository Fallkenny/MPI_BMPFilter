#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

/*-----------------------------------------------------*/
int main(int argc, char **argv){
	double termo, aux, p = 0;
	long i, ntermos;
	int id, np;
	MPI_Status s;
	double ti, tf;


	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &np);

	if ( id == 0 ){
		printf("Digite o numero de termos: ");
		scanf("%ld",&ntermos);
	}	
	
	ti = MPI_Wtime();


	if ( id == 0 ){
		for(i=1; i<np; i++){
			MPI_Send(&ntermos, 1, MPI_LONG, i, 100, MPI_COMM_WORLD);
		}
	}
	else{
		MPI_Recv(&ntermos, 1, MPI_LONG, 0, 100, MPI_COMM_WORLD, &s);
	}
	
	for( i=id+1; i<=ntermos; i+=np ){
		termo = 1.0 / (2*i-1);
		if ( i & 1 ){ 		
			p += termo;
		}
		else{
			p -= termo;
		} 
	}


	if ( id != 0 ){
		MPI_Send(&p, 1, MPI_DOUBLE, 0, 200, MPI_COMM_WORLD);
	}
	else{
		for(i=1; i<np; i++){
			MPI_Recv(&aux, 1, MPI_DOUBLE, i, 200, MPI_COMM_WORLD, &s);
			p += aux;
			
		}
		p *= 4;
	}

	tf = MPI_Wtime();

	if ( id == 0 ){
		printf("PI: %.15f\n", p);
		printf("Tempo: %f\n", tf - ti);

	}
	

	MPI_Finalize();


}
/*-----------------------------------------------------*/


