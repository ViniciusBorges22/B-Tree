#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arvore_b.h>

void tratamentoEntrada(char str[], char limite1, char limite2, int espaco);

int main()
{
	if(checagem(FALSE) == ERRO)
	{
		fprintf(stderr, "\nError checking data and index files");
		getchar();
		return ERRO;
	}
	int menu = TRUE;
	int id;
	char opt;
	char buffer[50];
	unsigned long byteoffset;
	tRegistro registro;

	while(menu){

		printf("\n MENU\n");
		printf("\n 1 - Create B-Tree from the data file");
		printf("\n 2 - Insert a new music");
		printf("\n 3 - Search by ID");
		printf("\n 4 - Print the b-tree");
		printf("\n 0 - Quit\n");
		printf("\n Option: ");

		fgets(buffer, 50, stdin);
		sscanf(buffer, "%c", &opt);
		char limitante2 = '0', limitante1 = '0';
		char auxid[50];

		switch(opt){

			case '1':
				if(checagem(TRUE) == ERRO)
				{
					fprintf(stderr, "\nError creating b-tree. Press any key to continue...");
					getchar();
				}
				break;

			case '2':
				printf("\n Title: ");
				char titulo[50];
				limitante1 = '0';
				limitante2 = 'z';
				tratamentoEntrada(titulo, limitante1, limitante2, TRUE);

				printf(" Genre: ");
				char genero[50];
				tratamentoEntrada(genero, limitante1, limitante2, TRUE);

				printf(" ID: ");

				limitante1 = '0';
				limitante2 = '9';
				tratamentoEntrada(auxid, limitante1, limitante2, FALSE);
				id = atoi(auxid);
				int retornoInsere = inserir(id, titulo, genero);
				if(retornoInsere == ERRO)
				{
					fprintf(stderr, "\nFile insertion error. Press any key to continue...");
					getchar();
				}
				break;

			case '3':
				printf("\n ID: ");

				limitante1 = '0';
				limitante2 = '9';
				tratamentoEntrada(auxid, limitante1, limitante2, FALSE);
				id = atoi(auxid);

				gravarLog("SEARCH operation execution by ID <%d>\n", id);

				if(busca(&registro, id, &byteoffset) == ENCONTRADO)
					gravarLog("Found key <%d>, offset <%li>, Title: <%s>, Genre: <%s>\n", id, byteoffset, registro.titulo, registro.genero);
				else
					gravarLog("Key <%d> not found\n", id);
				break;

			case '4':
				if(imprimeArvore() == ERRO)
				{
					fprintf(stderr, "\nError printing the b-tree. Press any key to continue...");
					getchar();
				}
				break;


			case '0':
				menu = FALSE;
				break;

			default:
				break;
		}
	}

	return 0;
}

/*
 *       Função que realiza o tratamento das entradas.
 *       Funcionamento:  Dada a entrada, salva em uma string auxiliar, percorre toda a string str checando se os caracteres
 *                       estão dentro do intervalo de código ASCII especificado. Se todos os caracteres estiverem, str é retornado
 *                       e a entrada está correta. Caso algum dos caracteres estiver fora do intervalo delimitado, é requisitada
 *                       uma nova entrada do usuário, e o processo é repetido.
 *       ARGUMENTOS:
 *                  char str[] = string que ser� salva o conteudo da leitura.
 *                  int limite1 e limite2 = Bordas menor e maior do intervalo de c�digo ASCII desejado.
 *                  int espaco = serve para saber se o caractere " " (espaco) está habilitado na entrada
 *
 */
void tratamentoEntrada(char str[], char limite1, char limite2, int espaco){
	int condicional = TRUE;
	int erro;
	int cont;
	while(condicional){

		erro = FALSE;
		cont = 0;
		fgets(str, 50, stdin);

		// Remove um possível newline e todos os caracteres restantes no stdin
		{
			char *pos;
			if ((pos=strchr(str, '\n')) != NULL)
				*pos = '\0';
			else
			{
				int c;
				while ((c = getchar()) != '\n' && c != EOF);
			}
		}

		// Strings vazias não são permitidas
		if(strlen(str) == 0)
			erro = TRUE;

		while(str[cont] != '\0' && erro == FALSE){
			if( !(str[cont] >= limite1 && str[cont] <= limite2) ){
				if(espaco){
					if(str[cont] != ' '){
						erro = TRUE;
					}
				}
				else
					erro = TRUE;
			}
			cont++;
		}

		if(erro == FALSE){
			condicional = FALSE;
		}
		else
			printf("\n Wrong input format (forbidden characters).\nTry again: ");
	}
	return;
}
