#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arvore_b.h"
#include "dados.h"

int busca(tRegistro* registro, int id, FILE* indice)
{
    int raiz;
    rewind(indice);                             //garante que o índice será lido do começo
    fread(&raiz, sizeof(int), 1, indice);       //lê do índice o RRN da raiz da árvore
    fseek(indice, raiz*sizeof(no), SEEK_CUR);   //posiciona o ponteiro do índice no registro da raiz
    no atual;                                   //nó auxiliar para buscar na árvore

    fread(&atual, sizeof(no), 1, indice);

    long byteoffset = buscaAux(id, indice, atual);  //função auxiliar, permite passar o nó atual como argumento para a recursão
    if(byteoffset == -1)                            //caso o byteoffset da chave buscada seja "-1" (não encontrada)
    {
        return 0;               //retorna o código de "chave não encontrada"
    }

    FILE* dados;
    if((dados = fopen("dados.dad", "rb")) == NULL)
    {
        fprintf(stderr, "Erro na abertura do arquivo de dados\n");
        return -1;              //código de erro
    }
    fseek(dados, byteoffset, SEEK_SET);         //posiciona o ponteiro do arquivo de dados no registro desejado
    char size;                                  //tamanho do registro
    fread(&size, sizeof(char), 1, dados);       //lê o tamanho do registro do arquivo
    char buffer[1000];
    fread(&buffer, size, 1, dados);             //lê o registro e armazena no buffer
    int pos = 0;
    sscanf(parser(buffer, &pos), "%d", &(registro->id));
    strcpy(registro->titulo, parser(buffer, &pos));
    strcpy(registro->genero, parser(buffer, &pos));
    return 1;                                   //código de "chave encontrada"
}

long buscaAux(int id, FILE* indice, no atual)
{
    int i;
    for(i = 0; i < atual.tam; i++)              //percorre todas as chaves do nó
    {
        if(id < atual.chaves[i].id)             //caso o id procurado seja menor que o id da chave atual, procura no filho à esquerda da chave atual
        {
            int rrn = atual.filhos[i];
            if(rrn == -1)                       //caso atual seja um nó folha, retorna o código de "chave não encontrada"
                return -1;
            fseek(indice, rrn*sizeof(no) + sizeof(int), SEEK_SET);  //posiciona o ponteiro do arquivo no registro do filho correspondente
                                                                    //"sizeof(int)" é somado ao offset para considerar o cabeçalho do índice
            fread(&atual, sizeof(no), 1, indice);   //o nó filho é armazenado no nó atual
            return buscaAux(id, indice, atual);     //chama recursivamente a função
        }
        else if(id > atual.chaves[i].id)
        {
            if(i == atual.tam - 1 || id < atual.chaves[i+1].id)           //caso o id procurado esteja entre duas chaves consecutivas, procura no filho à direita da chave atual
            {
                fseek(indice, (atual.filhos[i+1])*sizeof(no) + sizeof(int), SEEK_SET);    //posiciona o ponteiro do arquivo no registro do filho correspondente
                fread(&atual, sizeof(no), 1, indice);   //o nó filho é armazenado no nó atual
                return buscaAux(id, indice, atual);     //chama recursivamente a função
            }
            else                                //caso o id procurado seja maior que o id das duas chaves consecutivas, continua o loop
                continue;
        }
        else                                    //caso o id procurado seja igual ao id da chave atual, retorna o byteoffset dessa chave
        {
            return atual.chaves[i].byteoffset;
        }
    }
    return -1;
}

char *parser(char *buffer, int *pos) {
    int posi = *pos;

    while(buffer[*pos]!='|')
        (*pos)++;
    buffer[*pos] = '\0';
    (*pos)++;
    return &buffer[posi];
}

void inserir(int id, char nome[], char genero[]){

    FILE* fp = fopen("")

}




