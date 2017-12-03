#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arvore_b.h"

void tratamentoEntrada(char aux[], char buffer[], char limite1, char limite2, int espaco);

int main()
{
    if(checagem(FALSE) == ERRO)                                                         //
    {                                                                                   // Chama a função que faz a checagem da existência do arquivo de dados
        fprintf(stderr, "\nErro na checagem inicial dos arquivos de dados e indices."); // e a atualização do arquivo de índices (caso esteja desatualizado).
        getchar();                                                                      // Imprime uma mensagem caso ocorra algum erro durante o processo.
        return ERRO;                                                                    //
    }
    int menu = TRUE;
    int id, opt;
    char buffer[50];
    unsigned long byteoffset;
    tRegistro registro;

    while(menu){

        system("clear");
        printf("\n MENU\n");
        printf("\n 1 - Criar arvore a partir do arquivo de dados.");
        printf("\n 2 - Inserir novas musicas.");
        printf("\n 3 - Busca por id.");
        printf("\n 4 - Impressao da arvore.");
        printf("\n 0 - Sair\n");
        printf("\n Opcao: ");

        fgets(buffer, 50, stdin);
        sscanf(buffer, "%d", &opt);
        char limitante2 = '0', limitante1 = '0';
        char auxid[50];

        switch(opt){
            case 2:
                printf("Qual o titulo da musica a ser inserida?\n");
                char titulo[50];
                limitante1 = '0';     //C�digo ASCII da primero alpha numerico
                limitante2 = 'z';    //C�digo ASCII da ultimo alphanumerico
                tratamentoEntrada(titulo, buffer, limitante1, limitante2, TRUE);

                printf("Qual o genero da musica a ser inserida?\n");
                char genero[50];
                tratamentoEntrada(genero, buffer, limitante1, limitante2, TRUE);

                printf("Qual o id da musica a ser inserida?\n");

                limitante1 = '0';            //Codigo ASCII do numero 0.
                limitante2 = '9';            //Codigo ASCII do numero 9
                tratamentoEntrada(auxid, buffer, limitante1, limitante2, FALSE);
                id = atoi(auxid);       //Converte o auxid de string para int.
                int retornoInsere = inserir(id, titulo, genero);
                if(retornoInsere == ERRO)
                {
                    fprintf(stderr, "\nErro na insercao do arquivo. Pressione qualquer tecla para continuar...");
                    getchar();
                }
                break;

            case 3:
                printf("Qual o id da musica a ser buscada?\n");

                limitante1 = '0';            //Codigo ASCII do numero 0.
                limitante2 = '9';            //Codigo ASCII do numero 9
                tratamentoEntrada(auxid, buffer, limitante1, limitante2, FALSE);
                id = atoi(auxid);       //Converte o auxid de string para int.

                gravarLog("Execucao de operacao de PESQUISA de <%d>\n", id);

                if(busca(&registro, id, &byteoffset) == ENCONTRADO)
                    gravarLog("Chave <%d> encontrada, offset <%li>, Titulo: <%s>, Genero: <%s>\n", id, byteoffset, registro.titulo, registro.genero);
                else
                    gravarLog("Chave <%d> nao encontrada\n", id);
                break;

            case 4:
                if(imprimeArvore() == ERRO)
                {
                    fprintf(stderr, "\nErro na impressao da arvore. Pressione qualquer tecla para continuar...");
                    getchar();
                }
                break;

            case 1:
                if(checagem(TRUE) == ERRO)
                {
                    fprintf(stderr, "\nErro na criacao da arvore. Pressione qualquer tecla para continuar...");
                    getchar();
                }
                break;

            case 0:
                menu = FALSE;
                break;

            default:
                break;
        }
    }

    return 0;
}

/*
*       Fun��o que realiza o tratamento das entradas.
*       Funcionamento: L� a entrada, salva em uma string auxiliar, percorre toda a string aux checando se os caracteres
*                       est�o dentro do intervalo de codigo ASCII especificado, se todos os caracteres estiverem, aux � retornado
*                       e a entrada est� correta, caso algum dos caracteres estiver fora do intervalo delimitado, � requerida
*                       uma nova entrada do usu�rio, e o processo � repetido.
*       ARGUMENTOS:
*                  char aux[] = string que ser� salva o conteudo da leitura.
*                  char buffer[] = auxiliar para leitura
*                  int limite1 e limite2 = Bordas menor e maior do intervalo de c�digo ASCII desejado.
*                  int espaco = serve para saber se o caractere " " (espaco) está habilitado na entrada
*
*/
void tratamentoEntrada(char aux[], char buffer[], char limite1, char limite2, int espaco){
    int condicional = TRUE, erro, cont;
    while(condicional){
        erro = FALSE;
        cont = 0;
        fgets(buffer, 50, stdin);
        int tam = strlen(buffer) - 1;
        if (buffer[tam] == '\n')
            buffer[tam] = '\0';
        strcpy(aux, buffer);
        while(aux[cont] != '\0' && erro == FALSE){

            if( !(aux[cont] >= limite1 && aux[cont] <= limite2) ){
                if(espaco){
                    if(aux[cont] != 32){
                        erro = TRUE;
                    }
                    else
                        erro = FALSE;
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
            printf("Formato de entrada incorreto. Digite novamente.\n\t\tEntrada: ");
    }
    return;
}
