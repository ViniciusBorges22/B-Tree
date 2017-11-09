#include <stdio.h>
#include <stdlib.h>
#include "arvore_b.h"

#define true  1
#define false 0

int main()
{

    int menu = true;
    int opt;

    while(menu == true){

        printf("\n MENU\n");
        printf("\n 1 - Adicionar dado");
        printf("\n 2 - Buscar dado");
        printf("\n 3 - Remover dado");
        printf("\n 0 - Sair\n");
        printf("\n Opcao: ");

        scanf("%d", &opt);

        switch(opt){
            case 1:
                system("cls");
                //inserir
                break;
            case 2:
                system("cls");
                //buscar
                break;
            case 3:
                system("cls");
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
