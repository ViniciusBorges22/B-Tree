#ifndef ARVORE_B_H_INCLUDED
#define ARVORE_B_H_INCLUDED

#include "dados.h"

#define ORDEM 5 // Ordem da árvore

typedef int boolean;
#define TRUE 1
#define FALSE 0

#define ERRO -1          //
#define NAOENCONTRADO 0  // Códigos de retorno das funções
#define ENCONTRADO 1     //

#define PROMOCAO 2      // Códigos utilizados na função de inserção da árvore
#define NAOPROMOCAO 3   // para indicar se houve ou não promoção de uma chave

typedef struct
{
    int id;     //id da música
    unsigned long byteoffset;    //posição do registro desejado no arquivo de dados
}
chave;

typedef struct
{
    boolean folha;
    unsigned short tam;    //quantidade de chaves armazenadas na página
    chave chaves[ORDEM-1];    //vetor de chaves
    int filhos[ORDEM];   //"ponteiros" (RRN) para os filhos da página
}
pagina;     //página (nó) da árvore

int busca(tRegistro* registro, int id, FILE* indice);
long buscaAux(int id, FILE* indice, pagina atual);
char *parser(char *buffer, int *pos);
int buscaBinaria(chave chaves[], int id, int esq, int dir);
int inserir(int id, char titulo[], char genero[]);
int inserirArv(int RRN_atual, int id, int* promo, int* RRN_filho, FILE* indice, unsigned long byteoffsetReg);

#endif // ARVORE_B_H_INCLUDED
