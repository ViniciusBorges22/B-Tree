#include <stdio.h>
#include <stdlib.h>
#include "arvore_b.h"

#define true  1
#define false 0

int main()
{
    int menu = true;
    int opt;
    tRegistro registro;

    while(menu){

        system("cls");
        printf("\n MENU\n");
        printf("\n 1 - Adicionar dado");
        printf("\n 2 - Buscar dado");
        printf("\n 3 - Remover dado");
        printf("\n 0 - Sair\n");
        printf("\n Opcao: ");

        scanf("%d", &opt);

        switch(opt){
            case 1:
                printf(" %d", inserir(25, "so deus pode me julgar", "rap"));
                break;
            case 2:
                if(busca(&registro, 25) == NAOENCONTRADO)
                {
                    printf("Chave nao encontrada\n");
                    return 0;
                }
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
        getchar();
    }

    return 0;
}
