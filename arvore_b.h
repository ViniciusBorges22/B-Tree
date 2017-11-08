#ifndef ARVORE_B_H_INCLUDED
#define ARVORE_B_H_INCLUDED

#define ORDEM 5 //ordem da árvore

typedef struct
{
    int tam;    //quantidade de chaves armazenadas no nó
    int chaves[ORDEM-1];    //valor das chaves
    int ponteiros[ORDEM];   //ponteiros para os filhos do nó
}
no;

#endif // ARVORE_B_H_INCLUDED
