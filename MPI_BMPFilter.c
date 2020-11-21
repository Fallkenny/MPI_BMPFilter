#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

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

unsigned char calculaMediana(unsigned char *v, int tam)
{
	int i, j;
	unsigned char aux, mediana;

	for (i = 0; i < tam - 1; i++)
	{
		for (j = i + 1; j < tam; j++)
		{

			if (v[i] > v[j])
			{

				aux = v[i];
				v[i] = v[j];
				v[j] = aux;
			}
		}
	}

	if (tam % 2)
	{
		mediana = v[tam / 2];
	}
	else
	{
		mediana = (v[tam / 2 - 1] + v[tam / 2]) / 2;
	}
	return mediana;
}

void filtraPixel(int N, int x, int y, CABECALHO *cabecalho, char *imgOriginal, char *imgFiltrada)
{
	int volta = (N - 1) / 2;
	int size = N * N;
	unsigned char color[size];
	int i, j;

	int id = 0;
	for (j = y - volta; j <= y + volta; j++)
	{
		for (i = x - volta; i <= x + volta; i++)
		{
			if (i > -1 && j > -1 && i < cabecalho->altura && j < cabecalho->largura)
			{
				color[id] = imgOriginal[(i * cabecalho->largura) + j];
				id++;
			}
		}
	}

	imgFiltrada[(x * cabecalho->largura) + y] = calculaMediana(color, id);
	;
}

/*---------------------------------------------------------------------*/
int main(int argc, char **argv)
{
	int i, j;
	int id, np, nmascara;
	short media;
	char *imagem_entrada, *imagem_saida;
	CABECALHO cabecalho;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &np);
	MPI_Comm_rank(MPI_COMM_WORLD, &id);

	if (argc != 4)
	{
		if (id == 0)
			printf("%s <tamanho_mascara> <imagem_entrada> <imagem_saida>\n", argv[0]);

		MPI_Finalize();
		exit(0);
	}

	nmascara = atoi(argv[1]);

	imagem_entrada = argv[2];
	imagem_saida = argv[3];

	char *pixelsR, *filteredPixelsR, *pixelsAuxR, *filteredPixelsAuxR;
	char *pixelsG, *filteredPixelsG, *pixelsAuxG, *filteredPixelsAuxG;
	char *pixelsB, *filteredPixelsB, *pixelsAuxB, *filteredPixelsAuxB;

	RGB pixel;
	FILE *fin, *fout;

	fin = fopen(imagem_entrada, "rb");
	fout = fopen(imagem_saida, "wb");

	if (fin == NULL)
	{
		printf("Erro ao abrir o arquivo %s\n", imagem_entrada);
		exit(0);
	}

	if (fout == NULL)
	{
		printf("Erro ao abrir o arquivo %s\n", imagem_saida);
		exit(0);
	}

	fread(&cabecalho, sizeof(CABECALHO), 1, fin);

	printf("Tamanho da imagem: %u\n", cabecalho.tamanho_arquivo);
	printf("Largura: %d\n", cabecalho.largura);
	printf("Altura: %d\n", cabecalho.altura);
	printf("Bits por pixel: %d\n", cabecalho.bits_por_pixel);

	pixelsAuxR = (char *)malloc((cabecalho.altura / np) * cabecalho.largura * sizeof(char));
	pixelsAuxG = (char *)malloc((cabecalho.altura / np) * cabecalho.largura * sizeof(char));
	pixelsAuxB = (char *)malloc((cabecalho.altura / np) * cabecalho.largura * sizeof(char));
	filteredPixelsAuxR = (char *)malloc((cabecalho.altura / np) * cabecalho.largura * sizeof(char));
	filteredPixelsAuxG = (char *)malloc((cabecalho.altura / np) * cabecalho.largura * sizeof(char));
	filteredPixelsAuxB = (char *)malloc((cabecalho.altura / np) * cabecalho.largura * sizeof(char));

	if (id == 0)
	{
		pixelsR = (char *)malloc(cabecalho.altura * cabecalho.largura * sizeof(char));
		pixelsG = (char *)malloc(cabecalho.altura * cabecalho.largura * sizeof(char));
		pixelsB = (char *)malloc(cabecalho.altura * cabecalho.largura * sizeof(char));
		filteredPixelsR = (char *)malloc(cabecalho.altura * cabecalho.largura * sizeof(char));
		filteredPixelsG = (char *)malloc(cabecalho.altura * cabecalho.largura * sizeof(char));
		filteredPixelsB = (char *)malloc(cabecalho.altura * cabecalho.largura * sizeof(char));

		for (i = 0; i < cabecalho.altura; i++)
		{
			for (j = 0; j < cabecalho.largura; j++)
			{
				fread(
					&pixel,
					sizeof(RGB),
					1,
					fin);

				pixelsR[(i * cabecalho.largura) + j] = pixel.red;
				pixelsG[(i * cabecalho.largura) + j] = pixel.green;
				pixelsB[(i * cabecalho.largura) + j] = pixel.blue;
			}
		}
	}

	MPI_Scatter(
		pixelsR,
		cabecalho.altura / np * cabecalho.largura,
		MPI_CHAR,
		pixelsAuxR,
		cabecalho.altura / np * cabecalho.largura,
		MPI_CHAR,
		0,
		MPI_COMM_WORLD);
	MPI_Scatter(
		pixelsG,
		cabecalho.altura / np * cabecalho.largura,
		MPI_CHAR,
		pixelsAuxG,
		cabecalho.altura / np * cabecalho.largura,
		MPI_CHAR,
		0,
		MPI_COMM_WORLD);
	MPI_Scatter(
		pixelsB,
		cabecalho.altura / np * cabecalho.largura,
		MPI_CHAR,
		pixelsAuxB,
		cabecalho.altura / np * cabecalho.largura,
		MPI_CHAR,
		0,
		MPI_COMM_WORLD);

	for (i = 0; i < cabecalho.altura / np; i++)
	{
		for (j = 0; j < cabecalho.largura; j++)
		{
			filtraPixel(nmascara, i, j, &cabecalho, pixelsAuxR, filteredPixelsAuxR);
			filtraPixel(nmascara, i, j, &cabecalho, pixelsAuxG, filteredPixelsAuxG);
			filtraPixel(nmascara, i, j, &cabecalho, pixelsAuxB, filteredPixelsAuxB);
		}
	}

	MPI_Gather(
		filteredPixelsAuxR,
		cabecalho.altura / np * cabecalho.largura,
		MPI_CHAR,
		filteredPixelsR,
		cabecalho.altura / np * cabecalho.largura,
		MPI_CHAR,
		0,
		MPI_COMM_WORLD);

	MPI_Gather(
		filteredPixelsAuxG,
		cabecalho.altura / np * cabecalho.largura,
		MPI_CHAR,
		filteredPixelsG,
		cabecalho.altura / np * cabecalho.largura,
		MPI_CHAR,
		0,
		MPI_COMM_WORLD);

	MPI_Gather(
		filteredPixelsAuxB,
		cabecalho.altura / np * cabecalho.largura,
		MPI_CHAR,
		filteredPixelsB,
		cabecalho.altura / np * cabecalho.largura,
		MPI_CHAR,
		0,
		MPI_COMM_WORLD);

	if (id == 0)
	{
		fwrite(&cabecalho, sizeof(CABECALHO), 1, fout);

		//Escrever arquivo final
		for (i = 0; i < cabecalho.altura; i++)
		{
			for (j = 0; j < cabecalho.largura; j++)
			{
				pixel.red = filteredPixelsR[(i * cabecalho.largura) + j];
				pixel.green = filteredPixelsG[(i * cabecalho.largura) + j];
				pixel.blue = filteredPixelsB[(i * cabecalho.largura) + j];
				fwrite(&pixel, sizeof(RGB), 1, fout);
			}
		}
	}

	fclose(fin);
	fclose(fout);
	MPI_Finalize();
}
/*---------------------------------------------------------------------*/
