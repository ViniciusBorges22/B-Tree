#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "arvore_b.h"
#include "tad_fila.h"

//Função que busca uma chave na Árvore-B através de um ID passado como parâmetro.
//Em seguida, retorna o registro buscado e o byteoffset dele no arquivo de dados atrás de parâmetros.
int busca(tRegistro* registro, int id, unsigned long* byteoffset)
{
    FILE* indice;                                                   //
    if((indice = fopen("arvore.idx", "rb")) == NULL)                // Tenta abrir o arquivo de indice.
    {                                                               // Caso não consiga, imprime uma mensagem de erro e retorna ERRO.
        fprintf(stderr, "Erro na abertura do arquivo de indice\n"); //
        return ERRO;    //código de erro                            //
    }
    int raiz;                                                 // Carrega o RRN da raiz da Árvore-B e armazena na variável "raiz".
    carregaRaiz(&raiz, indice);                               //
    pagina atual;
    if(carregaPagina(&atual, raiz, indice) == NAOENCONTRADO)  // Utiliza o RRN acima para carregar a página referente à raiz.
    {
        fclose(indice);                                       // Caso o RRN passado como argumento for menor do que zero (não existe raiz)
        return NAOENCONTRADO;                                 // fecha o arquivo de indice e retorna "chave não encontrada".
    }
    chave buscaChave;                                         // Armazena numa variável do tipo "chave" o ID da chave a ser buscada
    buscaChave.id = id;                                       // Em seguida, chama uma função auxiliar que percorrerá a Árvore-B em busca
    if(buscaAux(atual, &buscaChave, indice) == NAOENCONTRADO) // da chave, a qual será armazenada em "buscaChave".
    {                                                         //
        fclose(indice);                                       // Caso essa função retorne "chave não encontrada", fecha o arquivo de indice
        return NAOENCONTRADO;                                 // e retorna NAOENCONTRADO.
    }
    fclose(indice);
    *byteoffset = buscaChave.byteoffset;                      // Escreve o byteoffset da chave encontrada no ponteiro passado como parâmetro.

    FILE* dados;                                                    //
    if((dados = fopen("dados.dad", "rb")) == NULL)                  // Tenta abrir o arquivo de dados.
    {                                                               // Caso não consiga, imprime uma mensagem de erro e retorna ERRO.
        fprintf(stderr, "Erro na abertura do arquivo de dados\n");  //
        return ERRO;    //código de erro                            //
    }
    fseek(dados, buscaChave.byteoffset, SEEK_SET);  // Posiciona o ponteiro do arquivo de dados no registro a ser recuperado.
    if(carregaRegistro(registro, dados) == ERRO)    // Carrega esse registro do arquivo e armazena no ponteiro passado como parâmetro.
        return NAOENCONTRADO;                       // Caso a leitura não seja bem sucedida, retorna "registro não encontrado".
    fclose(dados);                                  // Fecha o arquivo de dados.
    return ENCONTRADO;                              // Retorna o código para "chave e registro encontrados".
}

//Função de busca auxiliar que recebe uma página e executa a busca de uma chave a partir dessa página.
int buscaAux(pagina atual, chave* buscaChave, FILE* indice)
{
    int pos;                                                                            //
    while((pos = buscaBinaria(atual.chaves, buscaChave, 0, atual.tam-1)) != ENCONTRADO) // Executa uma busca binária no vetor de chaves da página.
    {                                                                                   // Armazena em "pos" a posição onde a chave buscada deveria estar.
        if(carregaPagina(&atual, atual.filhos[pos], indice) == NAOENCONTRADO)           // Acessa o filho da página atual na posição indicada por "pos".
            return NAOENCONTRADO;                                                       // Armazena a página filha em "atual" e reinicia o loop realizando
    }                                                                                   // a busca na nova página até que a chave seja encontrada.
    return pos;                                                                         // Caso "pos" seja "NAOENCONTRADO", retorna esse valor.
}                                                                                       //

//Função que executa uma busca binária em um vetor de chaves em busca de um ID.
//Retorna o byteoffset do registro correspondente através do ponteiro "novaChave".
int buscaBinaria(chave chaves[], chave* novaChave, int esq, int dir)
{
    if(esq == dir)                                          //
    {                                                       // Base da recursão: quando sobra apenas uma chave a ser comparada
        if(novaChave->id < chaves[esq].id)                  //
            return esq;                                     // Compara o ID a ser buscado com o ID da última chave a ser comparada.
        else if(novaChave->id > chaves[esq].id)             // Caso seja menor, retorna a posição à esquerda dessa chave.
            return esq+1;                                   // Caso seja maior, retorna a posição à direita dessa chave.
        else                                                //
        {                                                   // Caso contrário, ambos são iguais e a chave buscada já se encontra na Árvore-B.
            novaChave->byteoffset = chaves[esq].byteoffset; // Escreve o byteoffset da chave buscada no ponteiro "novaChave".
            return ENCONTRADO;                              // Retorna o código para "chave encontrada".
        }                                                   //
    }

    int mid = (esq + dir)/2;                        // Calcula a posição média do intervalo a ser buscado, utilizada para restringir a busca.

    if(novaChave->id < chaves[mid].id)                      //
   		return buscaBinaria(chaves, novaChave, esq, mid);   // Compara o ID a ser buscado com o ID da chave de posição média do vetor.
                                                            //
    else if(novaChave->id > chaves[mid].id)                 // Caso seja menor, chama a recursão para a metade esquerda do intervalo.
    	return buscaBinaria(chaves, novaChave, mid+1, dir); // Caso seja maior, chama a recursão para a metade direita do intervalo.
                                                            //
    else                                                    // Caso contrário, ambos são iguais e a chave buscada já se encontra na Árvore-B.
    {                                                       // Escreve o byteoffset da chave buscada no ponteiro "novaChave".
        novaChave->byteoffset = chaves[mid].byteoffset;     // Retorna o código para "chave encontrada".
        return ENCONTRADO;                                  //
    }                                                       //
}

//Função que lê a página do arquivo de indice a partir de um RRN passado como parâmetro.
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
        {                                                            //Caso a chave já se encontre na Árvore-B (retorne encontrado) pula para o próximo registro.
            byteoffset = ftell(dados);
            continue;
        }
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
