#ifndef TAD_FILA_H_INCLUDED
#define TAD_FILA_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>

#include <arvore_b.h>

typedef pagina elem;        //tipo de dados

typedef struct bloco{
    elem info;              //elemento da fila
    struct bloco* prox;     //ponteiro para o próximo
}no;                        //nó da fila

typedef struct{
   no* inicio;              //ponteiro para o inicio da fila
   no* fim;                 //ponteiro para o fim da fila
}fila;

void criarFila(fila* L);
int estaVaziaFila(fila* L);
int inserirFila(fila* L, elem pag);
int removerFila(fila* L, elem* info);

#endif // TAD_FILA_H_INCLUDED
