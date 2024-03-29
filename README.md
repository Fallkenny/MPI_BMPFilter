## Como compilar?

```
mpicc -o MPI_BMPFilter MPI_BMPFilter.c
```

## Como executar?

```
mpirun -np <n_processos> ./MPI_BMPFilter <tamanho_mascara> <imagem_entrada> <imagem_saida>
```

## Testes
### Imagem 1920x1080 
| Processos | Mascáras | Tempo (s) | SpeedUp | Eficiência |
|-----------|----------|-------|---------|------------|
| 1 | 3 | 1.634 | 1 | 1 | 
| 2 | 3 | 0.956 | 1.709205021 | 0.8546025105 |
| 3 | 3 | 0.707 | 2.311173975 | 0.7703913248 |
| 4 | 3 | 0.610 | 2.678688525 | 0.6696721311 |
| 1 | 5 | 8.209 | 1 | 1 |
| 2 | 5 | 4.151 | 1.97759576 | 0.98879788 |
| 3 | 5 | 2.812 | 2.919274538 | 0.9730915126 |
| 4 | 5 | 2.251 | 3.646823634 | 0.9117059085 |
| 1 | 7 | 25.340 | 1 | 1 |
| 2 | 7 | 13.022 | 1.945937644 | 0.972968822 |
| 3 | 7 | 8.890 | 2.850393701 | 0.9501312336 |
| 4 | 7 | 6.789 | 3.73250847 | 0.9331271174 |

### Imagem 600x400 
| Processos | Mascáras | Tempo (s) | SpeedUp | Eficiência |
|-----------|----------|-------|---------|------------|
| 1 | 3 | 0.219	| 1	| 1 | 
| 2 | 3 | 0.15	| 1.46	| 0.73 |
| 3 | 3 | 0.128	| 1.7109375	| 0.5703125 |
| 4 | 3 | 0.100	| 2.19	| 0.5475 |
| 1 | 5 | 0.99	| 1	| 1 |
| 2 | 5 | 0.572	| 1.730769231 | 0.8653846154 |
| 3 | 5 | 0.407	| 2.432432432 | 0.8108108108 |
| 4 | 5 | 0.34	| 2.911764706 | 0.7279411765 |
| 1 | 7 | 3.262	| 1	| 1 |
| 2 | 7 | 1.688	| 1.932464455	| 0.9662322275 |
| 3 | 7 | 1.162	| 2.807228916	| 0.9357429719 |
| 4 | 7 | 0.941	| 3.466524973	| 0.8666312434 |

