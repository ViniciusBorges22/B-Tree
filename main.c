#include <stdio.h>
#include <stdlib.h>
#include "arvore_b.h"

#define true  1
#define false 0

int main()
{
    int menu = true;
    int id, opt;
    char buffer[30], mensagem[50];
    long byteoffset;
    tRegistro registro;

    while(menu){

        system("clear");
        printf("\n MENU\n");
        printf("\n 1 - Adicionar dado");
        printf("\n 2 - Buscar dado");
        printf("\n 3 - Remover dado");
        printf("\n 0 - Sair\n");
        printf("\n Opcao: ");

        fgets(buffer, 30, stdin);
        sscanf(buffer, "%d", &opt);

        switch(opt){
            case 1:
                printf("Qual o titulo da musica a ser inserida?\n");
                char titulo[30];
                fgets(titulo, 30, stdin);
                printf("Qual o genero da musica a ser inserida?\n");
                char genero[20];
                fgets(genero, 20, stdin);
                printf("Qual o id da musica a ser inserida?\n");
                fgets(buffer, 30, stdin);
                sscanf(buffer, "%d", &id);

                sscanf(mensagem, "Execucao de operacao de INSERCAO de <%d>, <%s>, <%s>.\n", id, titulo, genero);
                gravarLog(mensagem);

                int retornoInsere = inserir(id, titulo, genero);
                system("clear");
                if(retornoInsere == ERRO)
                    fprintf(stderr, "\nErro na insercao do arquivo.");
                else if(retornoInsere == ENCONTRADO)
                {
                    sscanf(mensagem, "Chave <%d> duplicada\n", id);
                    gravarLog(mensagem);
                }
                else
                {
                    sscanf(mensagem, "Chave <%d> inserida com sucesso\n", id);
                    gravarLog(mensagem);
                }
                getchar();
                break;
            case 2:
                printf("Qual o id da musica a ser buscada?\n");
                fgets(buffer, 30, stdin);
        		sscanf(buffer, "%d", &id);

                sscanf(mensagem, "Execucao de operacao de PESQUISA de <%d>\n", id);
                gravarLog(mensagem);

                system("clear");
                if(busca(&registro, id, &byteoffset) == ENCONTRADO)
                {
                    sscanf(mensagem, "Chave <%d> encontrada, offset <%li>, Titulo: <%s>, Genero: <%s>\n", id, byteoffset, registro.titulo, registro.genero);
                    gravarLog(mensagem);
                }
                else
                {
                    sscanf(mensagem, "Chave <%d> nao encontrada\n", id);
                    gravarLog(mensagem);
                }
                getchar();
                break;

            case 3:
                //remover
                break;
            case 0:
                menu = false;
                break;
            default:
                break;
        }
    }

    return 0;
}
