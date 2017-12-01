#ifndef TAD_FILA_H_INCLUDED
#define TAD_FILA_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include "arvore_b.h"

#define TAMFILA 100

typedef pagina elem;

typedef struct {
    elem info[TAMFILA];
    int inicio;
    int fim;
    int total;
}Fila;

void CriaFila(Fila *F);
int EstaVaziaFila(Fila F);
int EstaCheiaFila(Fila F);
void EntraFila(Fila *F, elem item, int *erro);
void SairFila(Fila *F, elem *item, int *erro);
void EsvaziaFila(Fila *F);

#endif // TAD_FILA_H_INCLUDED
