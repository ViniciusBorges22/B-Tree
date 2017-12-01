#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dados.h"

char montarBuffer(int id, char titulo[], char genero[], char buffer[])
{
    sprintf(buffer, "%d|%s|%s|", id, titulo, genero);
    return (char) strlen(buffer);
}

void inserirArq(int id, char titulo[], char genero[], FILE* dados)
{
    char buffer[300];
    char tam = montarBuffer(id, titulo, genero, buffer);
    fwrite(&tam, sizeof(char), 1, dados);
    fwrite(&buffer, tam, 1, dados);
}

int proxRegistro(FILE* dados, tRegistro *registro)
{
    if(feof(dados)){
        return ERRO;
    }
    char size;                                  //tamanho do registro
    fread(&size, sizeof(char), 1, dados);       //lê o tamanho do registro do arquivo
    char buffer[1000];
    fread(&buffer, size, 1, dados);             //lê o registro e armazena no buffer
    int pos = 0;
    sscanf(parser(buffer, &pos), "%d", &(registro->id));
    strcpy(registro->titulo, parser(buffer, &pos));
    strcpy(registro->genero, parser(buffer, &pos));
    return ENCONTRADO;
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
