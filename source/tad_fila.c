#include <tad_fila.h>

void criarFila(fila* L){
    L->inicio = NULL;
    L->fim    = NULL;
}

int estaVaziaFila(fila* L){
    if(L->inicio == NULL){
        return TRUE;
    }
    return FALSE;
}

int inserirFila(fila* L, elem pag ){

    no* p;
    p = (no*)malloc(sizeof(no));

    if(p == NULL){
        return ERRO;
    }

    p->info = pag;
    p->prox = NULL;

    if(L->inicio == NULL){
        L->inicio = p;
    }
    else
        L->fim->prox = p;
    L->fim = p;

    return TRUE;

}

int removerFila(fila* L, elem* info)
{
    if(!estaVaziaFila(L)){
        if(L->inicio == L->fim)
            L->fim = NULL;
        no anterior;
        anterior.info = L->inicio->info;
        *info = anterior.info;
        anterior.prox = L->inicio->prox;
        free(L->inicio);
        L->inicio = anterior.prox;
        return TRUE;
    }
    return ERRO;

}
