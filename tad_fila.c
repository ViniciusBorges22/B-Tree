#include "tad_fila.h"

void CriaFila(Fila *F){
    F->fim = 0;
    F->inicio = 0;
    F->total = 0;
}

int EstaVaziaFila(Fila F){
    if(F.total == 0)
        return 1;
    else
        return 0;
}

int EstaCheiaFila(Fila F){
    if(F.total == TAMFILA )
        return 1;
    else
        return 0;
}

void EntraFila(Fila *F, elem item, int *erro){
    if(!EstaCheiaFila(*F)){
        *erro = 0;
        F->info[F->fim] = item;
        F->total ++;
        if(F->fim == TAMFILA - 1)
            F->fim = 0;
        else
            F->fim ++;
    }
    else
        *erro = 1;
}

void SairFila(Fila *F, elem *item, int *erro){
    if(!EstaVaziaFila(*F)){
        *erro = 0;
        *item = F->info[F->inicio];
        F->total --;
        if(F->inicio == TAMFILA -1) F->inicio = 0;
        else F->inicio++;
    }
}

void EsvaziaFila(Fila *F){
    F->total = 0;
    F->inicio = 0;
    F->fim = 0;
}
