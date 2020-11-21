#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <stddef.h>

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
	borboleta int altura;
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
	newPixel.red = calculaMediana(R, id);
	newPixel.green = calculaMediana(G, id);
	newPixel.blue = calculaMediana(B, id);

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

	RGB *pixels, *filteredPixels, *pixelsAux, *filteredPixelsAux;

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

	pixelsAux = (RGB *)malloc((cabecalho.altura / np) * cabecalho.largura * sizeof(RGB));
	filteredPixelsAux = (RGB *)malloc((cabecalho.altura / np) * cabecalho.largura * sizeof(RGB));

	const int nitems = 3;
	int blocklengths[3] = {sizeof(char), sizeof(char), sizeof(char)};
	MPI_Datatype types[3] = {MPI_CHAR, MPI_CHAR, MPI_CHAR};
	MPI_Datatype mpi_rgb_type;
	MPI_Aint offsets[3];

	offsets[0] = offsetof(RGB, red);
	offsets[1] = offsetof(RGB, green);
	offsets[2] = offsetof(RGB, blue);

	MPI_Type_create_struct(nitems, blocklengths, offsets, types, &mpi_rgb_type);
	MPI_Type_commit(&mpi_rgb_type);

	if (id == 0)
	{
		pixels = (RGB *)malloc(cabecalho.altura * cabecalho.largura * sizeof(RGB));
		filteredPixels = (RGB *)malloc(cabecalho.altura * cabecalho.largura * sizeof(RGB));

		for (i = 0; i < cabecalho.altura; i++)
		{
			for (j = 0; j < cabecalho.largura; j++)
			{
				fread(
					&pixel,
					sizeof(RGB),
					1,
					fin);

				pixels[(i * cabecalho.largura) + j] = pixel;
			}
		}
	}

	MPI_Scatter(
		pixels,
		cabecalho.altura / np * cabecalho.largura,
		mpi_rgb_type,
		pixelsAux,
		cabecalho.altura / np * cabecalho.largura,
		mpi_rgb_type,
		0,
		MPI_COMM_WORLD);

	for (i = 0; i < cabecalho.altura / np; i++)
	{
		for (j = 0; j < cabecalho.largura; j++)
		{
			filtraPixel(nmascara, i, j, &cabecalho, pixelsAux, filteredPixelsAux);
		}
	}

	MPI_Gather(
		filteredPixelsAux,
		cabecalho.altura / np * cabecalho.largura,
		mpi_rgb_type,
		filteredPixels,
		cabecalho.altura / np * cabecalho.largura,
		mpi_rgb_type,
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
				pixel = filteredPixels[(i * cabecalho.largura) + j];
				fwrite(&pixel, sizeof(RGB), 1, fout);
			}
		}
	}

	fclose(fin);
	fclose(fout);
	MPI_Finalize();
	MPI_Type_free(&mpi_rgb_type);
}
/*---------------------------------------------------------------------*/
