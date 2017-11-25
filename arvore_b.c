#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arvore_b.h"

int busca(tRegistro* registro, int id)
{
    FILE* indice;
    if((indice = fopen("arvore.idx", "rb")) == NULL)
    {
        fprintf(stderr, "Erro na abertura do arquivo de dados\n");
        return ERRO;              //código de erro
    }
    int raiz;
    fread(&raiz, sizeof(int), 1, indice);       //lê do índice o RRN da raiz da árvore
    fseek(indice, raiz*sizeof(pagina) + sizeof(int), SEEK_CUR);     //posiciona o ponteiro do índice no registro da raiz
                                                                    //sizeof(int) é somado para considerar o contador no cabeçalho do arquivo
    pagina atual;                               //página auxiliar para buscar na árvore
    fread(&atual, sizeof(pagina), 1, indice);   //Atribui a raiz à página "atual"

    long byteoffset = buscaAux(id, indice, atual);  //função auxiliar, permite passar a página atual como argumento para a recursão
    if(byteoffset == NAOENCONTRADO)
        return NAOENCONTRADO;               //retorna o código de "chave não encontrada"
    fclose(indice);

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
    fclose(dados);
    int pos = 0;
    sscanf(parser(buffer, &pos), "%d", &(registro->id));
    strcpy(registro->titulo, parser(buffer, &pos));
    strcpy(registro->genero, parser(buffer, &pos));
    return ENCONTRADO;                          //código de "chave encontrada"
}

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
            fseek(indice, rrn*sizeof(pagina) + 2*sizeof(int), SEEK_SET);  //posiciona o ponteiro do arquivo no registro do filho correspondente
                                                                        //"sizeof(int)" é somado ao offset para considerar o cabeçalho do índice
            fread(&atual, sizeof(pagina), 1, indice);   //a página filha é armazenado na página atual
            return buscaAux(id, indice, atual);         //chama recursivamente a função
        }
        else if(id > atual.chaves[i].id)
        {
            if(i == atual.tam - 1 || id < atual.chaves[i+1].id)           //caso o id procurado esteja entre duas chaves consecutivas, procura no filho à direita da chave atual
            {
                fseek(indice, (atual.filhos[i+1])*sizeof(pagina) + 2*sizeof(int), SEEK_SET);    //posiciona o ponteiro do arquivo no registro do filho correspondente
                fread(&atual, sizeof(pagina), 1, indice);   //a página filha é armazenada na página atual
                return buscaAux(id, indice, atual);         //chama recursivamente a função
            }
            else                                //caso o id procurado seja maior que o id das duas chaves consecutivas, continua o loop
                continue;
        }
        else                                    //caso o id procurado seja igual ao id da chave atual, retorna o byteoffset dessa chave
            return atual.chaves[i].byteoffset;
    }
    return NAOENCONTRADO;
}

char *parser(char *buffer, int *pos) {
    int posi = *pos;

    while(buffer[*pos]!='|')
        (*pos)++;
    buffer[*pos] = '\0';
    (*pos)++;
    return &buffer[posi];
}

int buscaBinaria(chave chaves[], chave novaChave, int esq, int dir)
{
    if(esq == dir)
    {
        if(novaChave.id < chaves[esq].id)
            return esq;
        if(novaChave.id > chaves[esq].id)
            return esq+1;
        else
            return ENCONTRADO;
    }
    if(esq - dir == 1)
        return dir;

    int mid = (esq + dir)/2;

    if(novaChave.id < chaves[mid].id)
   		return buscaBinaria(chaves, novaChave, esq, mid-1);

    else if(novaChave.id > chaves[mid].id)
    	return buscaBinaria(chaves, novaChave, mid+1, dir);

    if(novaChave.id > chaves[dir].id)
    	return dir+1;

    else if(novaChave.id < chaves[esq].id)
    	return esq;

    else
   		return ENCONTRADO;         //Chave já está na arvore

}

int inserir(int id, char titulo[], char genero[])
{
    printf("inserir");

    FILE* dados;
    if((dados = fopen("dados.dad", "a+b")) == NULL)
    {
        fprintf(stderr, "Erro na abertura do arquivo de dados\n");
        return ERRO;              //código de erro
    }
    fseek(dados, 0, SEEK_END);
    long byteoffsetReg = ftell(dados);
    printf(" %li", byteoffsetReg);
    chave novaChave;
    novaChave.id = id;
    novaChave.byteoffset = byteoffsetReg;
    int raiz;
    FILE* indice;
    if((indice = fopen("arvore.idx", "r+b")) == NULL)
    {
        if((indice = fopen("arvore.idx", "w+b")) == NULL)
        {
            fprintf(stderr, "Erro na criação do arquivo de indices\n");
            return ERRO;              //código de erro
        }
        int raiz = -1;
        int contador = 0;
        fwrite(&raiz, sizeof(int), 1, indice);
        fwrite(&contador, sizeof(int), 1, indice);
        rewind(indice);
    }
    fread(&raiz, sizeof(int), 1, indice);
    chave promo;
    int RRN_filho;
    int valorRetorno = inserirArv(raiz, novaChave, &promo, &RRN_filho, indice);
    if(valorRetorno == PROMOCAO)
    {
        pagina novaRaiz;
        novaRaiz.chaves[0] = promo;
        novaRaiz.filhos[0] = raiz;
        novaRaiz.filhos[1] = RRN_filho;
        novaRaiz.tam = 1;
        int ultimo_rrn;
        fseek(indice, sizeof(int), SEEK_SET);
        fread(&ultimo_rrn, sizeof(int), 1, indice);
        raiz = ultimo_rrn++;
        rewind(indice);
        fwrite(&raiz, sizeof(int), 1, indice);
        fwrite(&ultimo_rrn, sizeof(int), 1, indice);
        fseek(indice, raiz*sizeof(pagina), SEEK_CUR);
        fwrite(&novaRaiz, sizeof(pagina), 1, indice);
    }
    if(valorRetorno != ENCONTRADO)
        inserirArq(id, titulo, genero, dados);
    fclose(dados);
    fclose(indice);
    return valorRetorno;
}

/*
    Parâmetros da função 'inserirArv':

    RRN_atual: RRN da página que está sendo analisada
    novaChave: chave a ser inserida na árvore
    promo: chave promovida
    RRN_filho: RRN do filho direito da chave promovida
*/

int inserirArv(int RRN_atual, chave novaChave, chave* promo, int* RRN_filho, FILE* indice)
{
    printf(" inserirArv");

    if(RRN_atual == -1)
    {
        *promo = novaChave;
        *RRN_filho = -1;
        return PROMOCAO;
    }
    else
    {
        fseek(indice, RRN_atual*sizeof(pagina) + 2*sizeof(int), SEEK_SET);
        pagina atual;
        fread(&atual, sizeof(pagina), 1, indice);
        int pos = buscaBinaria(atual.chaves, novaChave, 0, atual.tam-1);
        if(pos == ENCONTRADO)     //se a chave já se encontra na árvore
            return ENCONTRADO;    //retorna código de "chave já existente"
        chave promoAux;
        int RRN_filhoAux;
        int valorRetorno = inserirArv(atual.filhos[pos], novaChave, &promoAux, &RRN_filhoAux, indice);
        if(valorRetorno == NAOPROMOCAO || valorRetorno == ENCONTRADO)
            return valorRetorno;
        else if(atual.tam < ORDEM-1)    //se há espaço na página atual
        {
            atualizaPagina(atual.chaves, atual.filhos, &atual.tam, promoAux, RRN_filhoAux);
            fseek(indice, RRN_atual*sizeof(pagina) + 2*sizeof(int), SEEK_SET);
            fwrite(&atual, sizeof(pagina), 1, indice);
            return NAOPROMOCAO;
        }
        else
        {
            pagina novaPagina;      // Nova página que será criada pelo split
            if(split(promoAux, RRN_filhoAux, &atual, &novaPagina, promo, RRN_filho) == ERRO){
              return ERRO;
            }
            fseek(indice, RRN_atual*sizeof(pagina) + 2*sizeof(int), SEEK_SET);
            fwrite(&atual, sizeof(pagina), 1, indice);
            fseek(indice, *RRN_filho*sizeof(pagina) + 2*sizeof(int), SEEK_SET);
            fwrite(&novaPagina, sizeof(pagina), 1, indice);
            return PROMOCAO;
        }
    }
}

/*
    Parâmetros da função split:

    novaChave: chave a ser inserida na página resultante do split
    RRN_filho: RRN da página filha à direita da nova chave
    atual: página onde será realizada o split
    novaPagina: nova página criada pelo split
    promo: chave promovida pelo split
    RRN_filho_promo: RRN da página filha à direita da chave promovida
    byteoffset: em qual byteoffset o registro correspondente ao "id" se encontra no arquivo de dados
*/

int split(chave novaChave, int RRN_filho, pagina* atual, pagina* novaPagina, chave* promo, int* RRN_filho_promo)
{
    printf(" split");

    paginaAux temp;
    int i;
    for(i = 0; i < atual->tam; i++)
    {
        temp.chaves[i] = atual->chaves[i];
        temp.filhos[i] = atual->filhos[i];
    }
    temp.filhos[atual->tam] = atual->filhos[atual->tam];
    temp.tam = atual->tam;
    atualizaPagina(temp.chaves, temp.filhos, &(temp.tam), novaChave, RRN_filho);
    novaPagina->tam = 0;
    FILE* indice;
    if((indice = fopen("arvore.idx", "r+b")) == NULL)
    {
        fprintf(stderr, "Erro na abertura do arquivo de indice\n");
        return ERRO;              //código de erro
    }
    fseek(indice, sizeof(int), SEEK_SET);
    int ultimo_rrn;
    fread(&ultimo_rrn, sizeof(int), 1, indice);
    int meio = temp.tam/2;
    *promo = temp.chaves[meio];
    *RRN_filho_promo = ultimo_rrn++;
    fseek(indice, sizeof(int), SEEK_SET);
    fwrite(&ultimo_rrn, sizeof(int), 1, indice);
    fclose(indice);
    for(i = 0; i < meio; i++)
    {
        atual->chaves[i] = temp.chaves[i];
        atual->filhos[i] = temp.filhos[i];
    }
    atual->filhos[meio] = temp.filhos[meio];
    atual->tam = meio;
    for(i = meio + 1; i < temp.tam; i++)
    {
        novaPagina->chaves[i] = temp.chaves[i];
        novaPagina->filhos[i] = temp.filhos[i];
    }
    novaPagina->filhos[temp.tam] = temp.filhos[temp.tam];
    novaPagina->tam = temp.tam - meio - 1;

    return TRUE;
}

void atualizaPagina(chave chaves[], int filhos[], unsigned short* tam, chave novaChave, int RRN_filho)
{
    printf(" atualizaPagina");

    int pos = buscaBinaria(chaves, novaChave, 0, *tam-1);
    shiftDireita(chaves, filhos, pos, *tam);
    chaves[pos] = novaChave;
    filhos[pos+1] = RRN_filho;
    (*tam)++;
}

void shiftDireita(chave chaves[], int filhos[], int inicial, int tam)
{
    printf(" shiftDireita");

    int i;
    for (i = tam; i > inicial; i--)
    {
        chaves[i] = chaves[i-1];
        filhos[i+1] = filhos[i];
    }
}
