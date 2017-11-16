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

    fread(&atual, sizeof(no), 1, indice);       //Atribui a raiz ao nó "autal"

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

int montarBuffer(tRegistro reg, char* buffer){
    sprintf(buffer, %d|%s|%s|, reg.id, reg.titulo, reg.genero);
    return strlen(buffer);
}

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
    fseek(fpIndice, RRNraiz*sizeof(no), SEEK_CUR);		 //posiciona o ponteiro do índice no registro da raiz

    no atual;                                    	 //auxiliar para buscar na árvore
    fread(&atual, sizeof(no), 1, fpIndice);         //Atribui a raiz ao nó "atual"
	int RRNaux = RRNraiz;

    if(!buscaFolha(&atual, id, fpIndice, &RRNaux)){
        return -1;
    }

    inserir();
}

void inserir(int id, int RRNfolha, long byteoffset){

	rewind(fpIndice);
    fseek(fpIndice, RRNfolha*sizeof(no), SEEK_CUR);

    no folha;

    fread(&folha, sizeof(no), 1, fpIndice);

	if(no.tam < ORDEM-1){
      	no.chaves[tam] = id;
        ordena(no.chaves);
        return;
	}
    else
        split()



}


int buscaFolha(no* atual, int id, FILE* fpIndice, int* atualRRN){

    if(atual->folha == TRUE){
        return TRUE;
    }

    int pos = buscaBinaria(atual->chaves, id, 0, atual->tam-1);

    if(pos == -1){
        return FALSE;			//Retorna erro pois a chave ja esta inserida.
    }

    *atualRRN = atual->filhos[pos];
    fseek(fpIndice, *(atualRRN)*sizeof(no) + sizeof(int), SEEK_SET);  //posiciona o ponteiro do arquivo no registro do filho correspondente
    fread(atual, sizeof(no), 1, fpIndice);

    return buscafolha(atual, id, fpIndice, atualRRN);
}


int buscaBinaria(chave chaves[], int id, int esq, int dir){

    int mid = (esq + dir)/2;

    if(chaves[mid] == id ){
   		return -1; 						  //Chave já está na arvore
   	}
    if(chaves[dir] < id){
    	return dir+1;
    }
    if(chaves[esq] > id){
    	return esq;
    }
    if( esq - dir == 1 ){
        return dir;
    }
    else if(id < chaves[mid]){
   		return buscaBinaria(chaves, id, esq, mid-1);
   	}
    else if(id > chaves[mid]){
    	return buscaBinaria(chaves, id, mid+1, dir);
    }
}
