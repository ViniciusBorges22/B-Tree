#include <stdio.h>
#include <stdlib.h>
#include "arvore_b.h"

#define true  1
#define false 0

int main()
{
    int menu = true;
    int id, opt;
    char buffer[30];
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
                char nome[30];
                fgets(nome, 30, stdin);
                printf("Qual o genero da musica a ser inserida?\n");
                char genero[20];
                fgets(genero, 20, stdin);
                printf("Qual o id da musica a ser inserida?\n");

                fgets(buffer, 30, stdin);
                sscanf(buffer, "%d", &id);
                int retornoInsere = inserir(id, nome, genero);
                system("clear");
                if(retornoInsere == ERRO)
                    fprintf(stderr, "\nErro na insercao do arquivo.");
                else if(retornoInsere == ENCONTRADO)
                    fprintf(stderr, "\nErro na insercao do arquivo: musica ja inserida.");
                else
                    printf("\nInsercao feita com sucesso.");
                getchar();
                break;
            case 2:
                printf("Qual o id da musica a ser buscada?\n");
                fgets(buffer, 30, stdin);
        		sscanf(buffer, "%d", &id);
                system("clear");
                if(busca(&registro, id) == ENCONTRADO)
                    printf("Titulo: %s\nGenero: %s\nID:[%d]\n", registro.titulo, registro.genero,registro.id);
                else
                    printf("\nMusica nao encontrada.");
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
