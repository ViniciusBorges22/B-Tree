#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arvore_b.h>

void tratamentoEntrada(char str[], char limite1, char limite2, int espaco);

int main()
{
    if(checagem(FALSE) == ERRO)
    {
        fprintf(stderr, "\nErro na checagem inicial dos arquivos de dados e indices.");
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
        printf("\n 1 - Criar arvore a partir do arquivo de dados.");
        printf("\n 2 - Inserir novas musicas.");
        printf("\n 3 - Busca por id.");
        printf("\n 4 - Impressao da arvore.");
        printf("\n 0 - Sair\n");
        printf("\n Opcao: ");

        fgets(buffer, 50, stdin);
        sscanf(buffer, "%c", &opt);
        char limitante2 = '0', limitante1 = '0';
        char auxid[50];

        switch(opt){

            case '1':
                if(checagem(TRUE) == ERRO)
                {
                    fprintf(stderr, "\nErro na criacao da arvore. Pressione qualquer tecla para continuar...");
                    getchar();
                }
                break;

            case '2':
                printf("\n Titulo: ");
                char titulo[50];
                limitante1 = '0';
                limitante2 = 'z';
                tratamentoEntrada(titulo, limitante1, limitante2, TRUE);

                printf(" Genero: ");
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
                    fprintf(stderr, "\nErro na insercao do arquivo. Pressione qualquer tecla para continuar...");
                    getchar();
                }
                break;

            case '3':
                printf("\n ID: ");

                limitante1 = '0';
                limitante2 = '9';
                tratamentoEntrada(auxid, limitante1, limitante2, FALSE);
                id = atoi(auxid);

                gravarLog("Execucao de operacao de PESQUISA de <%d>\n", id);

                if(busca(&registro, id, &byteoffset) == ENCONTRADO)
                    gravarLog("Chave <%d> encontrada, offset <%li>, Titulo: <%s>, Genero: <%s>\n", id, byteoffset, registro.titulo, registro.genero);
                else
                    gravarLog("Chave <%d> nao encontrada\n", id);
                break;

            case '4':
                if(imprimeArvore() == ERRO)
                {
                    fprintf(stderr, "\nErro na impressao da arvore. Pressione qualquer tecla para continuar...");
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
            printf("\n Formato de entrada incorreto. Digite novamente: ");
    }
    return;
}
