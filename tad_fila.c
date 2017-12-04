#include "tad_fila.h"

void criarFila(fila* L){
    L->inicio = NULL;           //inicializa inicio e fim como NULL
    L->fim    = NULL;
}

int estaVaziaFila(fila* L){
    if(L->inicio == NULL){      //se inicio for nulo fila est� vazia
        return TRUE;
    }
    return FALSE;
}

int inserirFila(fila* L, elem pag ){  //insere os elementos no final da fila

    no* p;                      //vetor no auxiliar para inserir a pagina
    p = (no*)malloc(sizeof(no));    //aloca�ao de memoria

    if(p == NULL){              //erro na aloca�ao de memoria
        return ERRO;
    }

    p->info = pag;          //salva a pagina no novo n� criado
    p->prox = NULL;         //seta o vetor de prox para NULL ja que sera o ultimo elemento

    if(L->inicio == NULL){      //se a fila estiver vazia define o inicio apontando para o no inserido
        L->inicio = p;
    }
    else                        //o no que era o ultimo da fila passa a apontar para o no inserido ja que ele � inserido no final da fila
        L->fim->prox = p;
    L->fim = p;                 //vetor fim aponta para o novo inserido

    return TRUE;

}

int removerFila(fila* L, elem* info)       // o elemento removido � sempre o primeiro da fila
{
    if(!estaVaziaFila(L)){          // checa se a fila ja nao est� vazia
        if(L->inicio == L->fim)     //se s� tiver um elemento na fila o fim ser� igual ao inicio e o ponteiro fim passar� a apontar para NULL
            L->fim = NULL;
        no anterior;            // no auxiliar para podermos dar free no n� que estava em primeiro e assim economizar memoria
        anterior.info = L->inicio->info;        //atribuindo as informa��es para nao perder dados
        *info = anterior.info;                  //carrega a pagina no ponteiro que foi passado no cabe�alho da fun��o
        anterior.prox = L->inicio->prox;
        free(L->inicio);                        //limpa da mem�ria o n� que era o primeiro da fila
        L->inicio = anterior.prox;          //estabelece o novo inicio.
        return TRUE;
    }
    return ERRO;       //Se a fila j� estiver vazia retorna ERRO

}
