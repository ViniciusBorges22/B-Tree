#ifndef ARVORE_B_H_INCLUDED
#define ARVORE_B_H_INCLUDED

#include <dados.h>

#define ORDEM 5 // Ordem da árvore.

#define TRUE 1
#define FALSE 0

#define ERRO -1           //
#define NAOENCONTRADO -2  // Valores de retorno das funções em geral.
#define ENCONTRADO -3     //

#define PROMOCAO -4      // Valores de retorno utilizados na função de inserção na.
#define NAOPROMOCAO -5   // árvore para indicar se houve ou não promoção de uma chave.

extern const char* btree_file;
extern const char* data_file;
extern const char* log_file;

typedef struct
{
	int id;                     // ID da música.
	unsigned long byteoffset;   // Posição do registro desejado no arquivo de dados.
}
chave;

typedef struct
{
	unsigned short tam;     // Quantidade de chaves armazenadas na página.
	chave chaves[ORDEM-1];  // Vetor de chaves.
	int filhos[ORDEM];      // "Ponteiros" (RRN) para os filhos da página.
}
pagina;     // Página (nó) da árvore.

typedef struct
{
	unsigned short tam;
	chave chaves[ORDEM];
	int filhos[ORDEM+1];
}
paginaAux;  // Página auxiliar utilizada na função split.


// Função que busca uma chave na Árvore-B através de um ID passado como parâmetro.
// Em seguida, retorna o registro buscado e o byteoffset dele no arquivo de dados atrás de parâmetros.
int busca(tRegistro* registro, int id, unsigned long* byteoffset);

// Função de busca auxiliar que recebe uma página e executa a busca de uma chave a partir dessa página.
int buscaAux(pagina atual, chave* buscaChave, FILE* indice);

// Função que executa uma busca binária em um vetor de chaves em busca de um ID.
// Retorna o byteoffset do registro correspondente através do ponteiro "novaChave".
int buscaBinaria(chave chaves[], chave* novaChave, int esq, int dir);

// Função que executa a inserção do registro no arquivo de dados e da sua chave correspondente no arquivo de indice.
int inserir(int id, char titulo[], char genero[]);

// Função auxiliar que executa a inserção da chave na Árvore-B.
int inserirAux(int id, unsigned long byteoffset);

// Função que insere recursivamente uma nova chave na Árvore-B.
int inserirArv(int RRN_atual, chave novaChave, chave* promo, int* RRN_filho, FILE* indice);

// Função que executa um 'split' de uma página e divide as chaves entre as páginas "atual" e "novaPagina", com promoção da chave mediana.
int split(chave novaChave, int RRN_filho, pagina* atual, pagina* novaPagina, chave* promo, int* RRN_filho_promo);

// Função que executa a impressão da Árvore-B no arquivo de log.
int imprimeArvore();

// Função auxiliar à "imprimeArvore".
// Recebe uma página e executa a impressão de seu nível, seu tamanho e suas chaves no arquivo de log.
void imprimePagina(pagina atual, int nivel);

// Função que lê a página do arquivo de indice a partir de um RRN passado como parâmetro.
int carregaPagina(pagina* atual, int RRN, FILE* indice);

// Função que lê o RRN da raiz da Árvore no cabeçalho do arquivo de indice.
void carregaRaiz(int* raiz, FILE* indice);

// Função que grava uma página no arquivo na posição indicada pelo RRN.
void escrevePagina(pagina atual, int RRN, FILE* indice);

// Função que atualiza (ou escreve pela primeira vez) o cabeçalho do arquivo de indice.
void escreveCabecalho(int raiz, int contador, FILE* indice);

// Função que grava no arquivo de log as operações executadas na Árvore-B.
int gravarLog(const char* format, ...);

// Função que executa uma checagem da existência do arquivo de indice.
// Caso não exista, cria o arquivo e inicializa o cabeçalho.
int verificaIndice();

// Função usada para construir (ou atualizar, caso já exista) o arquivo de indices a partir de um arquivo de dados já inserido.
int atualizaArvore();

// Função que insere uma nova chave em um vetor chaves na posição correta (seguindo a ordenação crescente).
void atualizaPagina(chave chaves[], int filhos[], unsigned short* tam, chave novaChave, int RRN_filho);

// Função que executa um shift a direita de todas as chaves posteriores a "inicio".
void shiftDireita(chave chaves[], int filhos[], int inicial, int tam);

#endif // ARVORE_B_H_INCLUDED
