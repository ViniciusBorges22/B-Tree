#ifndef DADOS_H_INCLUDED
#define DADOS_H_INCLUDED

#define ERRO -1          //
#define NAOENCONTRADO 0  // Códigos de retorno das funções
#define ENCONTRADO 1     //

typedef struct
{
    int id;
    char titulo[30];
    char genero[20];
}
tRegistro;

char montarBuffer(int id, char titulo[], char genero[], char buffer[]);
unsigned long inserirArq(int id, char titulo[], char genero[]);

#endif // DADOS_H_INCLUDED
