## Como compilar?

```
mpicc -o MPI_BMPFilter MPI_BMPFilter.c
```

## Como executar?

```
mpirun -np <n_processos> ./MPI_BMPFilter <tamanho_mascara> <imagem_entrada> <imagem_saida>
```