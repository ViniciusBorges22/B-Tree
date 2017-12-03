#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dados.h"
#include "arvore_b.h"

//Função que executa uma checagem geral dos arquivos de dados e de indice.
int checagem(int f1)
{
    FILE* dados;                                                      //
    if((dados = fopen("dados.dad", "r+b")) == NULL)                   //
    {                                                                 //
        if((dados = fopen("dados.dad", "w+b")) == NULL)               // Verifica se existe o arquivo de dados.
        {                                                             // Caso não exista, cria o arquivo e escreve o cabeçalho com a flag 1 (a Árvore-B está atualizada),
            fprintf(stderr, "Erro na criação do arquivo de dados\n"); // em seguida, chama a função que verifica o arquivo de indice e cria o mesmo caso não exista.
            return ERRO;    //código de erro                          // Caso o arquivo de dados exista, pula para o "else" abaixo
        }                                                             //
        escreveCabecalhoDados(1, dados);                              //
        if(verificaIndice() == ERRO)                                  //
            return ERRO;                                              //
    }
    else                                                              //
    {                                                                 //
        if(verificaIndice() == ERRO)                                  // Verifica se o arquivo de indice existe.
            return ERRO;                                              //
        if(!estaAtualizado(dados) || f1)                              // Em seguida, verifica no cabeçalho se a Árvore-B está atualizada.
        {                                                             // Caso não esteja, chama a função que atualiza a Árvore.
            if(atualizaArvore() == ERRO)                              // Caso não ocorra nenhum erro, retorna TRUE.
                return ERRO;    //código de erro                      //
        }                                                             //
    }
    fclose(dados);
    return TRUE;
}

//Função que insere o registro (formatado) no arquivo de dados.
void inserirArq(int id, char titulo[], char genero[], FILE* dados)
{
    char buffer[200];
    char tam = montarBuffer(id, titulo, genero, buffer);
    fwrite(&tam, sizeof(char), 1, dados);
    fwrite(&buffer, tam, 1, dados);
    escreveCabecalhoDados(0, dados);    //Atualiza o cabeçalho com a flag 0 (a Árvore-B não está atualizada).
}

//Função que atualiza o cabeçalho do arquivo de dados. A flag "atualizado" indica se a Árvore-B está atualizada em relação ao arquivo de dados.
void escreveCabecalhoDados(char atualizado, FILE* dados)
{
    rewind(dados);                               //Retorna o ponteiro para o começo do arquivo (cabeçalho).
    fwrite(&atualizado, sizeof(char), 1, dados); //Escreve a flag desejada no cabeçalho.
}

//Função que verifica no cabeçalho do arquivo se a Árvore-B está atualizada em relação ao arquivo de dados.
int estaAtualizado(FILE* dados)
{
    rewind(dados);                              // Retorna o ponteiro do arquivo para o começo (cabeçalho).
    char atualizado;                            // Varivável de flag.
    fread(&atualizado, sizeof(char), 1, dados); // Lê a variável de flag do arquivo.
    return (int) atualizado;                    // Retorna o valor da flag: 1 caso esteja atualizada ou 0 caso não esteja.
}

int carregaRegistro(tRegistro *registro, FILE* dados)
{
    char size;    //tamanho do registro
    if(!fread(&size, sizeof(char), 1, dados))  //lê o tamanho do registro do arquivo e checa se nao chegou em EOF
        return ERRO;
    char buffer[200];
    fread(&buffer, size, 1, dados);            //lê o registro e armazena no buffer
    int pos = 0;
    sscanf(parser(buffer, &pos), "%d", &(registro->id));
    strcpy(registro->titulo, parser(buffer, &pos));
    strcpy(registro->genero, parser(buffer, &pos));
    return ENCONTRADO;
}

char montarBuffer(int id, char titulo[], char genero[], char buffer[])
{
    sprintf(buffer, "%d|%s|%s|", id, titulo, genero);
    return (char) strlen(buffer);
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


/*
    As funções abaixo talvez sejam úteis
*/


/*
int inserirDado(tRegistro reg, FILE* fpDados, long* byteoffset){

    char buffer[100];
	char size = montarBuffer(reg, buffer);

    if(fpDados = fopen("dados.dad", ab) != NULL){
    	if(fgetpos( fpDados,  byteoffset ) != 0)
            return -1;
    	fwrite(size, sizeof(size), 1, fpDados);
        fwrite(buffer, size, 1, fpDados);
        return 1;
	}
    return -1;
}

int inserirIndice(int id, FILE* fpIndice, long byteoffsetReg){

    int RRNraiz;
    rewind(fpIndice);                               	   //garante que o índice será lido do começo
    fread(&RRNraiz, sizeof(int), 1, fpIndice);      		//lê do índice o RRN da raiz da árvore
    fseek(fpIndice, RRNraiz*sizeof(pagina), SEEK_CUR);		 //posiciona o ponteiro do índice no registro da raiz

    pagina atual;                                    	 //auxiliar para buscar na árvore
    fread(&atual, sizeof(pagina), 1, fpIndice);         //Atribui a raiz à página "atual"
	int RRNaux = RRNraiz;

    if(!buscaFolha(&atual, id, fpIndice, &RRNaux)){
        return -1;
    }

    inserir();
}

void inserir(int id, int RRNfolha, long byteoffset){

	rewind(fpIndice);
    fseek(fpIndice, RRNfolha*sizeof(pagina), SEEK_CUR);

    pagina folha;

    fread(&folha, sizeof(pagina), 1, fpIndice);

	if(pagina.tam < ORDEM-1){
      	pagina.chaves[tam] = id;
        ordena(pagina.chaves);
        return;
	}
    else
        split();
}

int buscaFolha(pagina* atual, int id, FILE* fpIndice, int* atualRRN){

    if(atual->folha == TRUE){
        return TRUE;
    }

    int pos = buscaBinaria(atual->chaves, id, 0, atual->tam-1);

    if(pos == -1){
        return FALSE;			//Retorna erro pois a chave ja esta inserida.
    }

    *atualRRN = atual->filhos[pos];
    fseek(fpIndice, *(atualRRN)*sizeof(pagina) + sizeof(int), SEEK_SET);  //posiciona o ponteiro do arquivo no registro do filho correspondente
    fread(atual, sizeof(pagina), 1, fpIndice);

    return buscafolha(atual, id, fpIndice, atualRRN);
}
*/
