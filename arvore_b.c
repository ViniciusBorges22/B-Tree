#include <stdio.h>
#include <stdlib.h>
#include "arvore_b.h"
#include "dados.h"

tRegistro busca(int id, FILE* indice)
{
    int raiz;
    rewind(indice);                             //garante que o índice será lido do começo
    fread(&raiz, sizeof(int), 1, indice);       //lê do índice o RRN da raiz da árvore
    fseek(indice, raiz*sizeof(no), SEEK_CUR);   //posiciona o ponteiro do índice no registro da raiz
    no atual;                                   //nó auxiliar para buscar na árvore
    fread(&atual, sizeof(no), 1, indice);       //lê do índice o nó raiz e atribui ao nó atual
    chave res = buscaAux(id, indice, atual);    //função auxiliar, permite que a página seja utilizada como argumento
    FILE* dados;
    if(dados = fopen("dados.dad", "rb") == NULL)
        fprintf(stderr, "Erro na abertura do arquivo de dados\n");

    fseek(dados, res.byteoffset, SEEK_SET);     //posiciona o ponteiro do arquivo de dados no registro desejado
    tRegistro registro;
    char size;                                  //tamanho do registro
    fread(&size, sizeof(size), 1, dados);       //lê o tamanho do registro do arquivo
    char buffer[1000];
    fread(&buffer, size, 1, dados);             //lê o registro e armazena no buffer
    armazena_reg(buffer, &registro);            //armazena os dados lidos nos campos correspondentes do registro
    return registro;                            //retorna o registro encontrado
}

chave buscaAux(int id, FILE* indice, no atual)
{
    int i;
    for(i = 0; i < atual.tam; i++)                  //percorre todas as chaves do nó
    {
        if(id < atual.chaves[i].id)                 //caso o id procurado seja menor que o id da chave atual, procura no filho à esquerda da chave atual
        {
            fseek(indice, (atual.filhos[i])*sizeof(no) + sizeof(int), SEEK_SET);    //posiciona o ponteiro do arquivo no registro do filho correspondente
                                                                                    //"sizeof(int)" é somado ao offset para considerar o cabeçalho do índice
            fread(&atual, sizeof(no), 1, indice);   //o nó filho é armazenado no nó atual
            buscarAux(id, indice, atual);           //chama recursivamente a função
        }
        else if(id > atual.chaves[i].id)
        {
            if(id < atual.chaves[i+1].id)           //caso o id procurado esteja entre duas chaves consecutivas, procura no filho à direita da chave atual
            {
                fseek(indice, (atual.filhos[i+1])*sizeof(no) + sizeof(int), SEEK_SET);    //posiciona o ponteiro do arquivo no registro do filho correspondente
                fread(&atual, sizeof(no), 1, indice);   //o nó filho é armazenado no nó atual
                buscarAux(id, indice, atual);           //chama recursivamente a função
            }
            else                                    //caso o id procurado seja maior que o id das duas chaves consecutivas, continua o loop
                continue;
        }
        else                                        //caso o id procurado seja igual ao id da chave atual, retorna essa chave
        {
            return atual.chaves[i];
        }
    }
}
