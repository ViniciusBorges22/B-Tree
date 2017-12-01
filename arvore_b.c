#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arvore_b.h"

int busca(tRegistro* registro, int id, long* byteoffset)
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
    chave buscaChave;
    buscaChave.id = id;
    if(buscaAux(atual, &buscaChave, indice) == NAOENCONTRADO)
        return NAOENCONTRADO;               //retorna o código de "chave não encontrada"
    fclose(indice);
    *byteoffset = buscaChave.byteoffset;

    FILE* dados;
    if((dados = fopen("dados.dad", "rb")) == NULL)
    {
        fprintf(stderr, "Erro na abertura do arquivo de dados\n");
        return ERRO;              //código de erro
    }
    fseek(dados, buscaChave.byteoffset, SEEK_SET);  //posiciona o ponteiro do arquivo de dados no registro desejado
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

int buscaAux(pagina atual, chave* buscaChave, FILE* indice)
{
    int pos;
    while((pos = buscaBinaria(atual.chaves, buscaChave, 0, atual.tam-1)) != ENCONTRADO)
    {
        if(carregaPagina(&atual, atual.filhos[pos], indice) == NAOENCONTRADO)
            return NAOENCONTRADO;
    }
    return pos;
}

int carregaPagina(pagina* atual, int RRN, FILE* indice)
{
    if(RRN == -1)
        return NAOENCONTRADO;
    fseek(indice, RRN*sizeof(pagina) + 2*sizeof(int), SEEK_SET);
    fread(atual, sizeof(pagina), 1, indice);
    return ENCONTRADO;
}

void escrevePagina(pagina atual, int RRN, FILE* indice)
{
    fseek(indice, RRN*sizeof(pagina) + 2*sizeof(int), SEEK_SET);
    fwrite(&atual, sizeof(pagina), 1, indice);
}

int gravarLog(char mensagem[])
{
    FILE* log;
    if((log = fopen("logVBorges.txt", "ab")) == NULL)
    {
        fprintf(stderr, "Erro na abertura do arquivo de dados\n");
        return ERRO;              //código de erro
    }
    fprintf(log, mensagem);
    fclose(log);
    return TRUE;
}

char *parser(char *buffer, int *pos)
{
    int posi = *pos;

    while(buffer[*pos]!='|')
        (*pos)++;
    buffer[*pos] = '\0';
    (*pos)++;
    return &buffer[posi];
}

int buscaBinaria(chave chaves[], chave* novaChave, int esq, int dir)
{
    if(esq == dir)
    {
        if(novaChave->id < chaves[esq].id)
            return esq;
        else if(novaChave->id > chaves[esq].id)
            return esq+1;
        else
        {
            novaChave->byteoffset = chaves[esq].byteoffset;
            return ENCONTRADO;      //Chave já está na arvore
        }
    }

    int mid = (esq + dir)/2;

    if(novaChave->id < chaves[mid].id)
   		return buscaBinaria(chaves, novaChave, esq, mid);

    else if(novaChave->id > chaves[mid].id)
    	return buscaBinaria(chaves, novaChave, mid+1, dir);

    else
    {
        novaChave->byteoffset = chaves[mid].byteoffset;
        return ENCONTRADO;      //Chave já está na arvore
    }
}

/*  Função usada para construir um arquivo de indices "Arvore", a partir de um arquivo de dados posteriormente inserido.
*   Ela funciona da seguinte forma:
*       Um laço percorre todo o arquivo de dados, armezenando os registros nele presente e os byteoffset de cada um deles
*          em cada ciclo desse laço o registro tenta ser inserido na arvore, o qual só será inserido se ainda n estiver
*          presente nela. A condiçao de parada do laço é o fim do arquivo de dados.
*       Em seguida é salvo no log que a ação foi concluida e o arquivo de dados é fechado.
*/
int checagem()
{
    tRegistro registro;       // Registro auxiliar para salvar as informações do arquivo de dados.
    FILE* dados;
    if((dados = fopen("dados.dad", "rb")) == NULL){     // tentativa de abrir o arquivo
        fprintf(stderr, "Erro na leitura do arquivo de dados\n");
        return ERRO;              //código de erro
    }
    long byteoffset = ftell(dados);         // Primeiro byteoffset do primeiro registro, necessita ser carregado aqui pois ao ler com a funçao proxRegistro andaremos com o ponteiro do arquivo e perderiamos o byteoffset desse primeiro registro
    while(proxRegistro(dados, &registro) != ERRO){  // funçao que le do arquivo de dados e salva a informação lida no ponteiro passado como argumento
        inserirAux(registro.id, registro.titulo, registro.genero, byteoffset);  //Funçao para inserir o registro na arvore
        byteoffset = ftell(dados);              //atualização do byteoffset do proximo registro do arqvuio, para uma posterior inserção
    }
    fclose(dados);      //fechamento do arquivo de dados
    gravarLog("Execucao da criacao do arquivo de indice arvore.idx com base no arquivo de dados dados.dad.\n\n");  //Escrita no Log
    return TRUE;
}

int inserir(int id, char titulo[], char genero[])
{
    FILE* dados;
    if((dados = fopen("dados.dad", "a+b")) == NULL)
    {
        fprintf(stderr, "Erro na abertura do arquivo de dados\n");
        return ERRO;              //código de erro
    }
    fseek(dados, 0, SEEK_END);
    long byteoffsetReg = ftell(dados);
    int retorno;
    if((retorno = inserirAux(id, titulo, genero, byteoffsetReg)) != ENCONTRADO)
        inserirArq(id, titulo, genero, dados);
    fclose(dados);
    return retorno;
}

int inserirAux(int id, char titulo[], char genero[], long byteoffsetReg)
{
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
	rewind(indice);
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
    if(RRN_atual == -1)
    {
        *promo = novaChave;
        *RRN_filho = -1;
        return PROMOCAO;
    }
    else
    {
        pagina atual;
        carregaPagina(&atual, RRN_atual, indice);
        int pos = buscaBinaria(atual.chaves, &novaChave, 0, atual.tam-1);
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
            escrevePagina(atual, RRN_atual, indice);
            return NAOPROMOCAO;
        }
        else
        {
            pagina novaPagina;      // Nova página que será criada pelo split
            if(split(promoAux, RRN_filhoAux, &atual, &novaPagina, promo, RRN_filho) == ERRO){
              return ERRO;
            }
            char mensagem[50];
            sprintf(mensagem, "Divisao de no - pagina %d\n", RRN_atual);
            gravarLog(mensagem);
            sprintf(mensagem, "Chave <%d> promovida\n", promo->id);
            gravarLog(mensagem);
            escrevePagina(atual, RRN_atual, indice);
            escrevePagina(novaPagina, *RRN_filho, indice);
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
    paginaAux temp;
    int i, j;
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
	i = meio + 1;
	j = 0;
	while(i < temp.tam)
	{
		novaPagina->chaves[j] = temp.chaves[i];
        novaPagina->filhos[j] = temp.filhos[i];
		i++;
		j++;
	}
	novaPagina->filhos[j] = temp.filhos[i];
    novaPagina->tam = temp.tam - meio - 1;
    return TRUE;
}

void atualizaPagina(chave chaves[], int filhos[], unsigned short* tam, chave novaChave, int RRN_filho)
{
    int pos = buscaBinaria(chaves, &novaChave, 0, *tam-1);
    shiftDireita(chaves, filhos, pos, *tam);
    chaves[pos] = novaChave;
    filhos[pos+1] = RRN_filho;
    (*tam)++;
}

void shiftDireita(chave chaves[], int filhos[], int inicial, int tam)
{
    int i;
    for (i = tam; i > inicial; i--)
    {
        chaves[i] = chaves[i-1];
        filhos[i+1] = filhos[i];
    }
}
