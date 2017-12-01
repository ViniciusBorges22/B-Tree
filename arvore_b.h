#ifndef ARVORE_B_H_INCLUDED
#define ARVORE_B_H_INCLUDED

#include "dados.h"

#define ORDEM 5 // Ordem da árvore

#define TRUE 1
#define FALSE 0

#define ERRO -1           //
#define NAOENCONTRADO -2  // Valores de retorno das funções em geral
#define ENCONTRADO -3     //

#define PROMOCAO -4      // Valores de retorno utilizados na função de inserção na
#define NAOPROMOCAO -5   // árvore para indicar se houve ou não promoção de uma chave

typedef struct
{
    int id;     // id da música
    long byteoffset;   // Posição do registro desejado no arquivo de dados
}
chave;

typedef struct
{
    unsigned short tam;    // Quantidade de chaves armazenadas na página
    chave chaves[ORDEM-1];    // Vetor de chaves
    int filhos[ORDEM];   // "Ponteiros" (RRN) para os filhos da página
}
pagina;     // Página (nó) da árvore

typedef struct
{
    unsigned short tam;
    chave chaves[ORDEM];
    int filhos[ORDEM+1];
}
paginaAux;  // Página auxiliar utilizada na função split

//int inicializa();
int busca(tRegistro* registro, int id, long* byteoffset);
int buscaAux(pagina atual, chave* buscaChave, FILE* indice);
int carregaPagina(pagina* atual, int RRN, FILE* indice);
void escrevePagina(pagina atual, int RRN, FILE* indice);
int gravarLog(char mensagem[]);
char *parser(char *buffer, int *pos);
int buscaBinaria(chave chaves[], chave* novaChave, int esq, int dir);
int checagem();
int inserir(int id, char titulo[], char genero[]);
int inserirAux(int id, char titulo[], char genero[], long byteoffsetReg);
int inserirArv(int RRN_atual, chave novaChave, chave* promo, int* RRN_filho, FILE* indice);
int split(chave novaChave, int RRN_filho, pagina* atual, pagina* novaPagina, chave* promo, int* RRN_filho_promo);
void atualizaPagina(chave chaves[], int filhos[], unsigned short* tam, chave novaChave, int RRN_filho);
void shiftDireita(chave chaves[], int filhos[], int inicial, int tam);

#endif // ARVORE_B_H_INCLUDED
