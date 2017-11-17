#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arvore_b.h"

long buscaAux(int id, FILE* indice, pagina atual)
{
    int i;
    for(i = 0; i < atual.tam; i++)              //percorre todas as chaves da página
    {
        if(id < atual.chaves[i].id)             //caso o id procurado seja menor que o id da chave atual, procura no filho à esquerda da chave atual
        {
            int rrn = atual.filhos[i];
            if(rrn == -1)                       //caso atual seja uma página folha, retorna o código de "chave não encontrada"
                return NAOENCONTRADO;
            fseek(indice, rrn*sizeof(pagina) + sizeof(int), SEEK_SET);  //posiciona o ponteiro do arquivo no registro do filho correspondente
                                                                    //"sizeof(int)" é somado ao offset para considerar o cabeçalho do índice
            fread(&atual, sizeof(pagina), 1, indice);   //a página filha é armazenado na página atual
            return buscaAux(id, indice, atual);     //chama recursivamente a função
        }
        else if(id > atual.chaves[i].id)
        {
            if(i == atual.tam - 1 || id < atual.chaves[i+1].id)           //caso o id procurado esteja entre duas chaves consecutivas, procura no filho à direita da chave atual
            {
                fseek(indice, (atual.filhos[i+1])*sizeof(pagina) + sizeof(int), SEEK_SET);    //posiciona o ponteiro do arquivo no registro do filho correspondente
                fread(&atual, sizeof(pagina), 1, indice);   //a página filha é armazenada na página atual
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
    return NAOENCONTRADO;
}

int busca(tRegistro* registro, int id, FILE* indice)
{
    int raiz;
    rewind(indice);                             //garante que o índice será lido do começo
    fread(&raiz, sizeof(int), 1, indice);       //lê do índice o RRN da raiz da árvore
    fseek(indice, raiz*sizeof(pagina), SEEK_CUR);   //posiciona o ponteiro do índice no registro da raiz
    pagina atual;                                   //página auxiliar para buscar na árvore

    fread(&atual, sizeof(pagina), 1, indice);       //Atribui a raiz à página "autal"

    unsigned long byteoffset = buscaAux(id, indice, atual);  //função auxiliar, permite passar a página atual como argumento para a recursão
    if(byteoffset == NAOENCONTRADO)
    {
        return NAOENCONTRADO;               //retorna o código de "chave não encontrada"
    }

    FILE* dados;
    if((dados = fopen("dados.dad", "rb")) == NULL)
    {
        fprintf(stderr, "Erro na abertura do arquivo de dados\n");
        return ERRO;              //código de erro
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
    return ENCONTRADO;                          //código de "chave encontrada"
}

char *parser(char *buffer, int *pos) {
    int posi = *pos;

    while(buffer[*pos]!='|')
        (*pos)++;
    buffer[*pos] = '\0';
    (*pos)++;
    return &buffer[posi];
}

int buscaBinaria(chave chaves[], int id, int esq, int dir)
{
    int mid = (esq + dir)/2;

    if(chaves[dir].id < id){
    	return dir+1;
    }
    if(chaves[esq].id > id){
    	return esq;
    }
    if( esq - dir == 1 ){
        return dir;
    }
    else if(id < chaves[mid].id){
   		return buscaBinaria(chaves, id, esq, mid-1);
   	}
    else if(id > chaves[mid].id){
    	return buscaBinaria(chaves, id, mid+1, dir);
    }
    else{
   		return ENCONTRADO;         //Chave já está na arvore
   	}
}

int inserir(int id, char titulo[], char genero[])
{
    unsigned long byteoffsetReg = inserirArq(id, titulo, genero);

    FILE* indice;
    if((indice = fopen("arvore.idx", "r+b")) == NULL)
    {
        fprintf(stderr, "Erro na abertura do arquivo de indice\n");
        return ERRO;              //código de erro
    }
    int raiz;
    fread(&raiz, sizeof(int), 1, indice);
    int promo = -1, RRN_filho = -1;
    if(inserirArv(raiz, id, &promo, &RRN_filho, indice, byteoffsetReg) == ENCONTRADO)
        return ENCONTRADO;      //chave já está inserida
    fclose(indice);
    return NAOENCONTRADO;       //inserido com sucesso
}

/*
    Parâmetros da função 'inserirArv':

    RRN_atual: RRN da página que está sendo analisada
    id: chave (id da música) a ser inserida na árvore
    promo: chave promovida
    RRN_filho: RRN do filho direito da chave promovida
*/

int inserirArv(int RRN_atual, int id, int* promo, int* RRN_filho, FILE* indice, unsigned long byteoffsetReg)
{
    if(RRN_atual == -1)
    {
        *promo = id;
        *RRN_filho = -1;
        return PROMOCAO;
    }
    else
    {
        fseek(indice, RRN_atual*sizeof(pagina) + sizeof(int), SEEK_SET);
        pagina atual;
        fread(&atual, sizeof(pagina), 1, indice);
        int pos = buscaBinaria(atual.chaves, id, 0, atual.tam - 1);
        if(pos == -1)            //se a chave já se encontra na árvore
            return ENCONTRADO;   //retorna código de "chave já existente"
        int promoAux;
        int RRN_filhoAux;
        int valor_retorno = inserirArv(atual.filhos[pos], id, &promoAux, &RRN_filhoAux, indice, byteoffsetReg);
        if(valor_retorno == NAOPROMOCAO || valor_retorno == ERRO)
            return valor_retorno;
        else if(atual.tam < ORDEM-1)    //se há espaço na página atual
        {
            atual.chaves[atual.tam].id = promoAux;
            atual.chaves[atual.tam].byteoffset = byteoffsetReg;
            atual.filhos[atual.tam+1] = RRN_filhoAux;
            atual.tam++;
            //ordena(atual.chaves, atual.tam);
            fseek(indice, RRN_atual*sizeof(pagina) + sizeof(int), SEEK_SET);
            fwrite(&atual, sizeof(pagina), 1, indice);
            return NAOPROMOCAO;
        }
        else
        {
            pagina novaPagina;      // Nova página criada pelo split
            //split(promoAux, RRN_filhoAux, &atual, &novaPagina, promo, RRN_filho, byteoffsetReg);
            fseek(indice, RRN_atual*sizeof(pagina) + sizeof(int), SEEK_SET);
            fwrite(&atual, sizeof(pagina), 1, indice);
            fseek(indice, *RRN_filho*sizeof(pagina) + sizeof(int), SEEK_SET);
            fwrite(&novaPagina, sizeof(pagina), 1, indice);
            return PROMOCAO;
        }
    }
}
