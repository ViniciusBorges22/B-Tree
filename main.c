#include <stdio.h>
#include <stdlib.h>
#include "arvore_b.h"


void tratamentoEntrada(char aux[], char buffer[], char limite1, char limite2);

int main()
{
    int menu = TRUE;
    int id, opt;
    char buffer[50], mensagem[50];
    long byteoffset;
    tRegistro registro;

    while(menu){

        system("cls");
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
                tratamentoEntrada(titulo, buffer, limitante1, limitante2);

                printf("Qual o genero da musica a ser inserida?\n");
                char genero[50];
                tratamentoEntrada(genero, buffer, limitante1, limitante2);

                printf("Qual o id da musica a ser inserida?\n");

                limitante1 = '0';            //Codigo ASCII do numero 0.
                limitante2 = '9';            //Codigo ASCII do numero 9
                tratamentoEntrada(auxid, buffer, limitante1, limitante2);
                id = atoi(auxid);       //Converte o auxid de string para int.

                int retornoInsere = inserir(id, titulo, genero);
                if(retornoInsere == ERRO)
                {
                    fprintf(stderr, "\nErro na insercao do arquivo. Pressione qualquer tecla para continuar...");
                    getchar();
                }
                break;

            case 3:
                printf("Qual o id da musica a ser inserida?\n");

                limitante1 = '0';            //Codigo ASCII do numero 0.
                limitante2 = '9';            //Codigo ASCII do numero 9
                tratamentoEntrada(auxid, buffer, limitante1, limitante2);
                id = atoi(auxid);       //Converte o auxid de string para int.

                sprintf(mensagem, "Execucao de operacao de PESQUISA de <%d>\n", id);
                gravarLog(mensagem);

                if(busca(&registro, id, &byteoffset) == ENCONTRADO)
                {
                    sprintf(mensagem, "Chave <%d> encontrada, offset <%li>, Titulo: <%s>, Genero: <%s>\n", id, byteoffset, registro.titulo, registro.genero);
                    gravarLog(mensagem);
                }
                else
                {
                    sprintf(mensagem, "Chave <%d> nao encontrada\n", id);
                    gravarLog(mensagem);
                }
                break;

            case 4:
                if(imprimeArvore() == ERRO)
                {
                    fprintf(stderr, "\nErro na impressao da arvore. Pressione qualquer tecla para continuar...");
                    getchar();
                }
                break;

            case 1:
                if(checagem() == ERRO)
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
*
*/
void tratamentoEntrada(char aux[], char buffer[], char limite1, char limite2){
    int condicional = TRUE, erro, cont;
    while(condicional){
        erro = FALSE;
        cont = 0;
        fgets(buffer, 50, stdin);
        sscanf(buffer, "%s", aux);

        while(aux[cont] != '\0' && erro == FALSE){
            erro = TRUE;

            if(aux[cont] >= limite1 && aux[cont] <= limite2)
                erro = FALSE;

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
