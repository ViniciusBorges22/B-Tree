#ifndef ARVORE_B_H_INCLUDED
#define ARVORE_B_H_INCLUDED

#include "dados.h"

#define ORDEM 5 //ordem da árvore

typedef int boolean;
#define TRUE 1
#define FALSE 0


typedef struct
{
    int id;     //id da música
    long byteoffset;    //posição do registro desejado no arquivo de dados
}
chave;

typedef struct
{
    boolean folha;
    short tam;    //quantidade de chaves armazenadas no nó
    chave chaves[ORDEM-1];    //vetor de chaves
    int filhos[ORDEM];   //"ponteiros" (RRN) para os filhos do nó
    int folha;
}
no;     //nó da árvore

int busca(tRegistro* registro, int id, FILE* indice);
long buscaAux(int id, FILE* indice, no atual);
char *parser(char *buffer, int *pos);

#endif // ARVORE_B_H_INCLUDED
