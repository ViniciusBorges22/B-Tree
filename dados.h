#ifndef DADOS_H_INCLUDED
#define DADOS_H_INCLUDED

#define ERRO -1           //
#define NAOENCONTRADO -2  // Valores de retorno das funções em geral
#define ENCONTRADO -3     //

typedef struct
{
    int id;
    char titulo[30];
    char genero[20];
}
tRegistro;

char montarBuffer(int id, char titulo[], char genero[], char buffer[]);
void inserirArq(int id, char titulo[], char genero[], FILE* dados);
int proxRegistro(FILE* dados, tRegistro *registro);
char *parser(char *buffer, int *pos);

#endif // DADOS_H_INCLUDED
