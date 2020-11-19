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

/*---------------------------------------------------------------------*/
int main(int argc, char **argv)
{
	int i, j;
	int id, np, nmascara;
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

	if (fin == NULL)
	{
		printf("Erro ao abrir o arquivo %s\n", imagem_entrada);
		exit(0);
	}

	fread(&cabecalho, sizeof(CABECALHO), 1, fin);

	printf("Tamanho da imagem: %u\n", cabecalho.tamanho_arquivo);
	printf("Largura: %d\n", cabecalho.largura);
	printf("Largura: %d\n", cabecalho.altura);
	printf("Bits por pixel: %d\n", cabecalho.bits_por_pixel);

	RGB *pixels = malloc(cabecalho.altura * cabecalho.largura * sizeof(RGB));
	RGB pixel;

	for (i = 0; i < cabecalho.altura; i++)
	{
		for (j = 0; j < cabecalho.largura; j++)
		{
			fread(
				&pixels[(i * cabecalho.largura) + j],
				sizeof(RGB),
				1,
				fin);
		}
	}

	fclose(fin);
	// fclose(fout);
	// MPI_Finalize();
}
/*---------------------------------------------------------------------*/
