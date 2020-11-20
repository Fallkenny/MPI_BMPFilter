#include <stdio.h>
#include <stdlib.h>
//#include <mpi.h>

/*---------------------------------------------------------------------*/
#pragma pack(1)

/*---------------------------------------------------------------------*/
struct cabecalho
{
	unsigned short tipo;
	unsigned int tamanho_arquivo;
	unsigned short reservado1;
	unsigned short reservado2;
	unsigned int offset;
	unsigned int tamanho_image_header;
	int largura;
	int altura;
	unsigned short planos;
	unsigned short bits_por_pixel;
	unsigned int compressao;
	unsigned int tamanho_imagem;
	int largura_resolucao;
	int altura_resolucao;
	unsigned int numero_cores;
	unsigned int cores_importantes;
};
typedef struct cabecalho CABECALHO;

struct rgb
{
	unsigned char blue;
	unsigned char green;
	unsigned char red;
};
typedef struct rgb RGB;


unsigned char calculaMediana(unsigned char* v, int tam)
{
	int i,j;
	unsigned char aux, mediana;

	 for(i=0;i<tam-1;i++){
        for(j=i+1;j<tam;j++){
        	
            if(v[i] > v[j]){
            	
                aux = v[i];
                v[i] = v[j];
                v[j] = aux;
            }
        }
    }

    if(tam%2){
         mediana = v[tam/2];
    } else {
         mediana = (v[tam/2-1]+v[tam/2])/2;
    }
	return mediana;
}

void filtraPixel(int N, int x, int y, CABECALHO *cabecalho, RGB *imgOriginal, RGB *imgFiltrada)
{
	int volta = (N - 1) / 2;
	int size = N * N;
	unsigned char R[size];
	unsigned char G[size];
	unsigned char B[size];
	int i, j;

	int id = 0;
	for (j = y - volta; j <= y + volta; j++)
	{
		for (i = x - volta; i <= x + volta; i++)
		{
			if (i > -1 && j > -1 && i < cabecalho->altura && j < cabecalho->largura)
			{
				R[id] = imgOriginal[(i * cabecalho->largura) + j].red;
				G[id] = imgOriginal[(i * cabecalho->largura) + j].green;
				B[id] = imgOriginal[(i * cabecalho->largura) + j].blue;
				id++;
			}
		}
	}
	RGB newPixel;
	newPixel.red = calculaMediana(R,id);
	newPixel.green = calculaMediana(G,id);
	newPixel.blue = calculaMediana(B,id);

	imgFiltrada[(x * cabecalho->largura) + y] = newPixel;
}

/*---------------------------------------------------------------------*/
int main(int argc, char **argv)
{
	int i, j;
	int id, np, nmascara;
	short media;
	char *imagem_entrada, *imagem_saida;
	CABECALHO cabecalho;
	// MPI_Status s;

	// MPI_Init(&argc, &argv);
	// MPI_Comm_size(MPI_COMM_WORLD, &np);
	// MPI_Comm_rank(MPI_COMM_WORLD, &id);

	if (argc != 4)
	{
		if (id == 0)
			printf("%s <tamanho_mascara> <imagem_entrada> <imagem_saida>\n", argv[0]);
		// MPI_Finalize();

		exit(0);
	}

	nmascara = atoi(argv[1]);

	imagem_entrada = argv[2];
	imagem_saida = argv[3];

	FILE *fin = fopen(imagem_entrada, "rb");
	FILE *fout = fopen(imagem_saida, "wb");


	if (fin == NULL)
	{
		printf("Erro ao abrir o arquivo %s\n", imagem_entrada);
		exit(0);
	}

	if ( fout == NULL ){
		printf("Erro ao abrir o arquivo %s\n", imagem_saida);
		exit(0);
	}  

	fread(&cabecalho, sizeof(CABECALHO), 1, fin);

	printf("Tamanho da imagem: %u\n", cabecalho.tamanho_arquivo);
	printf("Largura: %d\n", cabecalho.largura);
	printf("Largura: %d\n", cabecalho.altura);
	printf("Bits por pixel: %d\n", cabecalho.bits_por_pixel);

	RGB *pixels = malloc(cabecalho.altura * cabecalho.largura * sizeof(RGB));
	RGB *filteredPixels = malloc(cabecalho.altura * cabecalho.largura * sizeof(RGB));
	RGB pixel;

	for (i = 0; i < cabecalho.altura; i++)
	{
		for (j = 0; j < cabecalho.largura; j++)
		{
			fread(
				&pixel,
				sizeof(RGB),
				1,
				fin);
			// media = (pixel.red + pixel.green + pixel.blue) / 3;
			// pixel.red = media;
			// pixel.green = media;
			// pixel.blue = media;
			pixels[(i * cabecalho.largura) + j] = pixel;
		}
	}

	for (i = 0; i < cabecalho.altura; i++)
	{
		for (j = 0; j < cabecalho.largura; j++)
		{
			filtraPixel(nmascara, i, j, &cabecalho, pixels, filteredPixels);
		}
	}


	fwrite(&cabecalho, sizeof(CABECALHO), 1, fout);

	//Escrever arquivo final
	for(i=0; i<cabecalho.altura; i++)
	{
		for(j=0; j<cabecalho.largura; j++){
			
			fwrite(&filteredPixels[(i * cabecalho.largura) + j], sizeof(RGB), 1, fout);
		}


	}

	fclose(fin);
	// fclose(fout);
	// MPI_Finalize();
}
/*---------------------------------------------------------------------*/
