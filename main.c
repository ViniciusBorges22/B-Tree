#include <stdio.h>
#include <stdlib.h>
#include "arvore_b.h"

#define true  1
#define false 0

int main()
{
    int menu = true;
    int opt;
    FILE* indice;

    while(menu){

        printf("\n MENU\n");
        printf("\n 1 - Adicionar dado");
        printf("\n 2 - Buscar dado");
        printf("\n 3 - Remover dado");
        printf("\n 0 - Sair\n");
        printf("\n Opcao: ");

        scanf("%d", &opt);

        system("clear");

        switch(opt){
            case 1:
                printf(" %d", inserir(25,"oloooco", "rap"));
                break;
            case 2:
                indice = fopen("arvore.idx", "rb");

                tRegistro registro;

                if(busca(&registro, 25, indice) == NAOENCONTRADO)
                {
                    printf("Chave nao encontrada\n");
                    return 0;
                }

                fclose(indice);

                printf("ID: %d\nNome: %s\nGenero: %s\n", registro.id, registro.titulo, registro.genero);
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
