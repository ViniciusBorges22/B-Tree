#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "arvore_b.h"
#include "tad_fila.h"

int busca(tRegistro* registro, int id, unsigned long* byteoffset)
{
    FILE* indice;
    if((indice = fopen("arvore.idx", "rb")) == NULL)
    {
        fprintf(stderr, "Erro na abertura do arquivo de indices\n");
        return ERRO;              //código de erro
    }
    int raiz;
    carregaRaiz(&raiz, indice);
    pagina atual;                               //página auxiliar para buscar na árvore
    if(carregaPagina(&atual, raiz, indice) == NAOENCONTRADO)
    {
        fclose(indice);
        return NAOENCONTRADO;
    }
    chave buscaChave;
    buscaChave.id = id;
    if(buscaAux(atual, &buscaChave, indice) == NAOENCONTRADO)
    {
        fclose(indice);
        return NAOENCONTRADO;   //retorna o código de "chave não encontrada"
    }
    fclose(indice);
    *byteoffset = buscaChave.byteoffset;

    FILE* dados;
    if((dados = fopen("dados.dad", "rb")) == NULL)
    {
        fprintf(stderr, "Erro na abertura do arquivo de dados\n");
        return ERRO;              //código de erro
    }
    fseek(dados, buscaChave.byteoffset, SEEK_SET);  //posiciona o ponteiro do arquivo de dados no registro desejado
    if(carregaRegistro(registro, dados) == ERRO)
        return NAOENCONTRADO;
    fclose(dados);
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

int carregaPagina(pagina* atual, int RRN, FILE* indice)
{
    if(RRN < 0)
        return NAOENCONTRADO;
    fseek(indice, RRN*sizeof(pagina) + 2*sizeof(int), SEEK_SET);
    fread(atual, sizeof(pagina), 1, indice);
    return ENCONTRADO;
}

void carregaRaiz(int* raiz, FILE* indice)
{
    rewind(indice);
    fread(raiz, sizeof(int), 1, indice);
}

void escrevePagina(pagina atual, int RRN, FILE* indice)
{
    fseek(indice, RRN*sizeof(pagina) + 2*sizeof(int), SEEK_SET);
    fwrite(&atual, sizeof(pagina), 1, indice);
}

void escreveCabecalho(int raiz, int ultimo_rrn, FILE* indice)
{
    rewind(indice);
    fwrite(&raiz, sizeof(int), 1, indice);
    fwrite(&ultimo_rrn, sizeof(int), 1, indice);
}

int gravarLog(const char* format, ...)
{
    FILE* log;
    if((log = fopen("logVBorges.txt", "a")) == NULL)
    {
        fprintf(stderr, "Erro na abertura do arquivo de log\n");
        return ERRO;              //código de erro
    }
    va_list args;
    va_start(args, format);
    vfprintf(log, format, args);
    va_end(args);
    fclose(log);
    return TRUE;
}

int verificaIndice()
{
    FILE* indice;
    if((indice = fopen("arvore.idx", "r+b")) == NULL)
    {
        if((indice = fopen("arvore.idx", "w+b")) == NULL)
        {
            fprintf(stderr, "Erro na criação do arquivo de indices\n");
            return ERRO;              //código de erro
        }
        int raiz = -1;
        int ultimo_rrn = 0;
        escreveCabecalho(raiz, ultimo_rrn, indice);
    }
    fclose(indice);
    return TRUE;
}

/*  Função usada para construir um arquivo de indices "Arvore", a partir de um arquivo de dados posteriormente inserido.
*   Ela funciona da seguinte forma:
*       Um laço percorre todo o arquivo de dados, armezenando os registros nele presente e os byteoffset de cada um deles
*          em cada ciclo desse laço o registro tenta ser inserido na arvore, o qual só será inserido se ainda n estiver
*          presente nela. A condiçao de parada do laço é o fim do arquivo de dados.
*       Em seguida é salvo no log que a ação foi concluida e o arquivo de dados é fechado.
*/
int atualizaArvore()
{
    gravarLog("Execucao da criacao do arquivo de indice arvore.idx com base no arquivo de dados dados.dad.\n");  //Escrita no Log
    FILE* dados;
    if((dados = fopen("dados.dad", "rb")) == NULL){     //Tentativa de abrir o arquivo
        fprintf(stderr, "Erro na leitura do arquivo de dados\n");
        return ERRO;              //código de erro
    }
    fseek(dados, sizeof(char), SEEK_SET);                            //Posiciona o ponteiro do arquivo após o cabeçalho de dados
    unsigned long byteoffset = ftell(dados);                         //Byteoffset do primeiro registro
    tRegistro registro;                                              //Registro auxiliar para salvar as informações do arquivo de dados.
    while(carregaRegistro(&registro, dados) != ERRO){                //Funçao que le do arquivo de dados e salva a informação lida no ponteiro passado como argumento
        if(busca(&registro, registro.id, &byteoffset) == ENCONTRADO) //Busca o ID obtido na Árvore-B.
            continue;                                                //Caso a chave já se encontre na Árvore-B (retorne encontrado) pula para o próximo registro.
        inserirAux(registro.id, byteoffset);                         //Funçao para inserir o registro na arvore
        byteoffset = ftell(dados);                                   //Atualização do byteoffset do proximo registro do arquivo, para uma posterior inserção
    }
    fclose(dados);      //Fechamento do arquivo de dados
    return TRUE;
}

int inserir(int id, char titulo[], char genero[])
{
    gravarLog("Execucao de operacao de INSERCAO de <%d>, <%s>, <%s>.\n", id, titulo, genero);
    tRegistro registro;
    unsigned long byteoffset;
    if(busca(&registro, id, &byteoffset) == ENCONTRADO)
    {
        gravarLog("Chave <%d> duplicada.\n", id);
        return ENCONTRADO;
    }
    FILE* dados;
    if((dados = fopen("dados.dad", "r+b")) == NULL)
    {
        fprintf(stderr, "Erro na abertura do arquivo de dados\n");
        return ERRO;              //código de erro
    }
    fseek(dados, 0, SEEK_END);
    byteoffset = ftell(dados);
    inserirArq(id, titulo, genero, dados);
    fflush(dados);
    getchar();
    int valorRetorno = inserirAux(id, byteoffset);
    escreveCabecalhoDados(1, dados);                //Atualiza o cabeçalho de dados com a flag 1 (a Árvore-B está atualizada).
    fclose(dados);
    return valorRetorno;
}

int inserirAux(int id, unsigned long byteoffset)
{
    chave novaChave;
    novaChave.id = id;
    novaChave.byteoffset = byteoffset;
    int raiz;
    FILE* indice;
    if((indice = fopen("arvore.idx", "r+b")) == NULL)
    {
        fprintf(stderr, "Erro na abertura do arquivo de indices\n");
        return ERRO;              //código de erro
    }
	carregaRaiz(&raiz, indice);
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
        escreveCabecalho(raiz, ultimo_rrn, indice);
        escrevePagina(novaRaiz, raiz, indice);
    }
    if(valorRetorno != ERRO)
        gravarLog("Chave <%d> inserida com sucesso.\n", id);
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
        chave promoAux;
        int RRN_filhoAux;
        int valorRetorno = inserirArv(atual.filhos[pos], novaChave, &promoAux, &RRN_filhoAux, indice);
        if(valorRetorno == NAOPROMOCAO)
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
            gravarLog("Divisao de no - pagina %d\n", RRN_atual);
            gravarLog("Chave <%d> promovida\n", promo->id);
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
        fprintf(stderr, "Erro na abertura do arquivo de indices\n");
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

int imprimeArvore()
{
    gravarLog("Execucao de operacao para mostrar a arvore-B gerada:\n");

    Fila f;
    CriaFila(&f);
    FILE* indice;
    if((indice = fopen("arvore.idx", "rb")) == NULL)
    {
        fprintf(stderr, "Erro na abertura do arquivo de indices\n");
        return ERRO;              //código de erro
    }
    int raiz;
    carregaRaiz(&raiz, indice);
    pagina atual;
    if(carregaPagina(&atual, raiz, indice) == NAOENCONTRADO)
    {
        gravarLog("Erro: Arvore-B nao encontrada\n");
        fclose(indice);
        return NAOENCONTRADO;
    }
    int erro = 0;
    EntraFila(&f, atual, &erro);
    int nivel = 0;
    int cont = 1;
    int somaCont = 0;
    while(!EstaVaziaFila(f) && !erro)
    {
        if(cont == 0)
        {
            nivel++;
            cont = somaCont;
            somaCont = 0;
        }
        SairFila(&f, &atual, &erro);
        imprimePagina(atual, nivel);
        cont--;
        int i;
        pagina aux;
        for(i = 0; i <= atual.tam && !erro; i++)
        {
            if(carregaPagina(&aux, atual.filhos[i], indice) == NAOENCONTRADO)
                break;
            EntraFila(&f, aux, &erro);
        }
        somaCont += atual.tam + 1;
    }
    fclose(indice);
    if(erro)
        return ERRO;
    else
        return TRUE;
}

void imprimePagina(pagina atual, int nivel)
{
    char mensagem[200];
    sprintf(mensagem, "%d %hu ", nivel, atual.tam);
    int i;
    char chave[50];
    for(i = 0; i < atual.tam; i++)
    {
        sprintf(chave, "<%d/%li> ", atual.chaves[i].id, atual.chaves[i].byteoffset);
        strcat(mensagem, chave);
    }
    strcat(mensagem, "\n");
    gravarLog(mensagem);
}
