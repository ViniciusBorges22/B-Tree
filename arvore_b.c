#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "arvore_b.h"
#include "tad_fila.h"

// Função que busca uma chave na Árvore-B através de um ID passado como parâmetro.
// Em seguida, retorna o registro buscado e o byteoffset dele no arquivo de dados atrás de parâmetros.
int busca(tRegistro* registro, int id, unsigned long* byteoffset)
{
    FILE* indice;                                                   //
    if((indice = fopen("arvore.idx", "rb")) == NULL)                // Tenta abrir o arquivo de indice.
    {                                                               //
        fprintf(stderr, "Erro na abertura do arquivo de indice\n"); // Caso não consiga, imprime uma mensagem de erro e retorna ERRO.
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
    {                                                               //
        fprintf(stderr, "Erro na abertura do arquivo de dados\n");  // Caso não consiga, imprime uma mensagem de erro e retorna ERRO.
        return ERRO;    //código de erro                            //
    }
    fseek(dados, buscaChave.byteoffset, SEEK_SET);  // Posiciona o ponteiro do arquivo de dados no registro a ser recuperado.
    if(carregaRegistro(registro, dados) == ERRO)    // Carrega esse registro do arquivo e armazena no ponteiro passado como parâmetro.
        return NAOENCONTRADO;                       // Caso a leitura não seja bem sucedida, retorna "registro não encontrado".
    fclose(dados);                                  // Fecha o arquivo de dados.
    return ENCONTRADO;                              // Retorna o código para "chave e registro encontrados".
}

// Função de busca auxiliar que recebe uma página e executa a busca de uma chave a partir dessa página.
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

// Função que executa uma busca binária em um vetor de chaves em busca de um ID.
// Retorna o byteoffset do registro correspondente através do ponteiro "novaChave".
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

// Função que lê a página do arquivo de indice a partir de um RRN passado como parâmetro.
int carregaPagina(pagina* atual, int RRN, FILE* indice)
{
    if(RRN < 0)                                                  // Se o RRN for -1 (Árvore vazia ou filho de uma folha)
        return NAOENCONTRADO;                                    // retorna código para "página não encontrada".
    fseek(indice, RRN*sizeof(pagina) + 2*sizeof(int), SEEK_SET); // Posiciona o ponteiro do arquivo na posição da página indicada pelo RRN.
    fread(atual, sizeof(pagina), 1, indice);                     // Lê a página do arquivo e armazena no ponteiro "atual".
    return ENCONTRADO;                                           // Retorna código para "página encontrada".
}

// Função que lê o RRN da raiz da Árvore no cabeçalho do arquivo de indice.
void carregaRaiz(int* raiz, FILE* indice)
{
    rewind(indice);                         // Posiciona o ponteiro no arquivo no começo do cabeçalho.
    fread(raiz, sizeof(int), 1, indice);    // Lê o primeiro inteiro correspondente ao RRN da raiz e armazena no ponteiro "raiz".
}

// Função que grava uma página no arquivo na posição indicada pelo RRN.
void escrevePagina(pagina atual, int RRN, FILE* indice)
{
    fseek(indice, RRN*sizeof(pagina) + 2*sizeof(int), SEEK_SET);  // Posiciona o ponteiro do arquivo na posição desejada de escrita.
    fwrite(&atual, sizeof(pagina), 1, indice);                    // Escreve a página "atual" nessa posição do.
}

// Função que atualiza (ou escreve pela primeira vez) o cabeçalho do arquivo de indice.
// Argumentos:
//  raiz: RRN da raiz da Árvore-B.
//  ultimo_rrn: maior RRN da Árvore-B, utilizada para criar novas páginas no split e na atualização da raiz.
void escreveCabecalho(int raiz, int ultimo_rrn, FILE* indice)
{
    rewind(indice);                                 // Posiciona o ponteiro no arquivo no começo do cabeçalho.
    fwrite(&raiz, sizeof(int), 1, indice);          // Escreve a variável raiz na primeira posição do cabeçalho.
    fwrite(&ultimo_rrn, sizeof(int), 1, indice);    // Escreve a variável ultimo_rrn na segunda posição do cabeçalho.
}

// Função que grava no arquivo de log as operações executadas na Árvore-B.
// Para isso, ela recebe uma string ("mensagem") a ser formatada posteriormente pelos possíveis argumentos recebidos à direita dela, similar ao printf.
// Nesta função são utilizados os recursos da biblioteca stdarg.
int gravarLog(const char* mensagem, ...)
{
    FILE* log;                                                           //
    if((log = fopen("logVBorges.txt", "a")) == NULL)                     // Tenta abrir o arquivo de log.
    {                                                                    //
        fprintf(stderr, "Erro na abertura/criação do arquivo de log\n"); // Caso não consiga, imprime uma mensagem de erro e retorna ERRO.
        return ERRO;    //código de erro                                 //
    }
    va_list argumentos;                         // Variável do tipo va_list com os argumentos a serem formatados na string da mensagem.
    va_start(argumentos, mensagem);             // Inicializa a variável "argumentos" com os argumentos adicionais passados na chamada da função.
    vfprintf(log, mensagem, argumentos);        // Imprime a mensagem formatada no arquivo log.
    va_end(argumentos);                         // Chamada necessária para encerrar as operações feitas por va_start() antes do retorno da função.
    fclose(log);                                // Fecha o arquivo de log.
    return TRUE;                                // Retorna TRUE para indicar que não houve erro.
}

// Função que executa uma checagem da existência do arquivo de indice.
// Caso não exista, cria o arquivo e inicializa o cabeçalho.
int verificaIndice()
{
    FILE* indice;                                                       //
    if((indice = fopen("arvore.idx", "r+b")) == NULL)                   // Tenta abrir o arquivo de indice.
    {                                                                   //
        if((indice = fopen("arvore.idx", "w+b")) == NULL)               // Caso ele não exista, tenta criar o mesmo.
        {                                                               //
            fprintf(stderr, "Erro na criação do arquivo de indice\n");  // Caso não consiga criar, imprime uma mensagem de erro e
            return ERRO;    //código de erro                            // retorna o código de erro.
        }                                                               //
        int raiz = -1;                                                  // Caso consiga criar, inicializa a raiz com -1
        int ultimo_rrn = 0;                                             // e o ultimo_rrn com 0.
        escreveCabecalho(raiz, ultimo_rrn, indice);                     // Inicializa a raiz e o ultimo_rrn no cabeçalho do arquivo.
    }                                                                   //
    fclose(indice);                                                     // Fecha o arquivo de indice.
    return TRUE;                                                        // Retorna TRUE para indicar que não houve erro.
}

// Função usada para construir (ou atualizar, caso já exista) o arquivo de indices a partir de um arquivo de dados já inserido.
// Um laço percorre todo o arquivo de dados, armezenando os registros nele presente e os byteoffset de cada um deles
//  em cada ciclo desse laço o registro tenta ser inserido na arvore, o qual só será inserido se não estiver
//  presente nela. A condiçao de parada do laço é o fim do arquivo de dados.
int atualizaArvore()
{
    gravarLog("Execucao da criacao do arquivo de indice arvore.idx com base no arquivo de dados dados.dad.\n");  //Escrita no Log
    FILE* dados;
    if((dados = fopen("dados.dad", "rb")) == NULL){                  // Tentativa de abrir o arquivo
        fprintf(stderr, "Erro na leitura do arquivo de dados\n");    // Caso não consiga, imprime mensagem de erro e retorna ERRO.
        return ERRO;    //código de erro
    }
    fseek(dados, sizeof(char), SEEK_SET);                            //Posiciona o ponteiro do arquivo após o cabeçalho de dados.
    unsigned long byteoffset = ftell(dados);                         //Lê o byteoffset do primeiro registro.
    tRegistro registro;                                              //Registro auxiliar para salvar as informações do arquivo de dados.
    while(carregaRegistro(&registro, dados) != ERRO){                //Funçao que le do arquivo de dados e salva a informação lida no ponteiro passado como argumento.
        if(busca(&registro, registro.id, &byteoffset) == ENCONTRADO) //Busca o ID obtido na Árvore-B.
        {                                                            //Caso a chave já se encontre na Árvore-B (a busca retorne encontrado)
            byteoffset = ftell(dados);                               // armazena o byteoffset do próximo registro
            continue;                                                // e reinicia o loop (pulando a inserção na Árvore).
        }
        inserirAux(registro.id, byteoffset);                         //Chama a função que insere o registro na Árvore.
        byteoffset = ftell(dados);                                   //Atualização do byteoffset do proximo registro do arquivo, para uma posterior inserção.
    }
    fclose(dados);                            // Fecha o arquivo de dados
    return TRUE;                              // Retorna TRUE para indicar que não houve erro.
}

// Função que executa a inserção do registro no arquivo de dados e da sua chave correspondente no arquivo de indice.
int inserir(int id, char titulo[], char genero[])
{
    gravarLog("Execucao de operacao de INSERCAO de <%d>, <%s>, <%s>.\n", id, titulo, genero); // Grava no arquivo de log a operação de inserção.
    tRegistro registro;                                            //
    unsigned long byteoffset;                                      // Declara variáveis de registro e de byteoffset.
    if(busca(&registro, id, &byteoffset) == ENCONTRADO)            // Executa uma busca do ID desejado passando o endereço dessas variáveis como parâmetros.
    {
        gravarLog("Chave <%d> duplicada.\n", id);                  // Caso a busca retorne "chave encontrada",
        return ENCONTRADO;                                         // não executa a inserção e retorna ENCONTRADO.
    }
    FILE* dados;                                                   //
    if((dados = fopen("dados.dad", "r+b")) == NULL)                // Tenta abrir o arquivo de indice.
    {                                                              //
        fprintf(stderr, "Erro na abertura do arquivo de dados\n"); // Caso não consiga, imprime uma mensagem de erro e retorna ERRO.
        return ERRO;    //código de erro                           //
    }
    fseek(dados, 0, SEEK_END);                     // Posiciona o ponteiro do arquivo no final do mesmo, onde será inserido o registro.
    byteoffset = ftell(dados);                     // Armazena o byteoffset dessa posição na variável "byteoffset".
    inserirArq(id, titulo, genero, dados);         // Chama a função que insere o registro no arquivo de dados.
    fflush(dados);                                 // Executa um fflush no arquivo para garantir que a flag de atualizado foi escrita (não se encontra no buffer).
    int valorRetorno = inserirAux(id, byteoffset); // Chama a função que insere a chave correspondente no arquivo de indice.
    escreveCabecalhoDados(1, dados);               // Atualiza o cabeçalho de dados com a flag 1 (a Árvore-B está atualizada).
    fclose(dados);                                 // Fecha o arquivo de dados.
    return valorRetorno;                           // Retorna o valor de retorno da função executa a inserção na Árvore-B.
}

// Função auxiliar que executa a inserção da chave na Árvore-B.
// Foi necessário criá-la para que não houvesse conflito entre as opções 1 e 2 do main, pois a opção 1 deve inserir apenas na Árvore.
// Além disso, esta função atualiza a raiz da Árvore caso haja uma promoção da raiz anterior durante a execução da "inserirArv".
int inserirAux(int id, unsigned long byteoffset)
{
    chave novaChave;                    // Cria uma nova chave.
    novaChave.id = id;                  // Armazena nela os dados a serem inseridos.
    novaChave.byteoffset = byteoffset;  //
    int raiz;                           // Variável que indica o RRN da raiz da Árvore.
    FILE* indice;
    if((indice = fopen("arvore.idx", "r+b")) == NULL)               // Tenta abrir o arquivo de indice.
    {                                                               //
        fprintf(stderr, "Erro na abertura do arquivo de indice\n"); // Caso não consiga, imprime mensagem de erro e retorna ERRO.
        return ERRO;    //código de erro                            //
    }
	carregaRaiz(&raiz, indice);                                     // Carrega o RRN da raiz da Árvore e armazena em "raiz".
    chave promo;                                                    // Chave promovida, será setada função "inserirArv".
    int RRN_filho;                                                  // RRN do filho direito da chave promovida, também será setada na função abaixo.
    int valorRetorno = inserirArv(raiz, novaChave, &promo, &RRN_filho, indice); // Chama a função que executa recursivamente a inserção da nova chave.
    if(valorRetorno == PROMOCAO)
    {                                                               // Caso o retorno da função indique que houve uma promoção da raiz.
        pagina novaRaiz;                                            // Cria uma nova raiz.
        novaRaiz.chaves[0] = promo;                                 // Armazena a chave promovida na primeira posição da nova raiz.
        novaRaiz.filhos[0] = raiz;                                  // O filho esquerdo da nova raiz será a raiz antiga.
        novaRaiz.filhos[1] = RRN_filho;                             // O filho direito da nova raiz será a nova página criada pelo split, indicado por "RRN_filho".
        novaRaiz.tam = 1;                                           // A nova raiz começa com tamanho igual a 1.
        int ultimo_rrn;                                             // Variável do último RRN disponível na Árvore.
        fseek(indice, sizeof(int), SEEK_SET);                       // Posiciona o ponteiro do arquivo de indice onde está armazenado o último RRN.
        fread(&ultimo_rrn, sizeof(int), 1, indice);                 // Lê o último RRN do arquivo.
        raiz = ultimo_rrn++;                                        // O RRN da raiz será o último RRN disponível. Após essa atribução, o último RRN é incrementado.
        escreveCabecalho(raiz, ultimo_rrn, indice);                 // Atualiza o cabeçalho com os novos valores de raiz e ultimo_rrn.
        escrevePagina(novaRaiz, raiz, indice);                      // Escreve a nova página raiz no arquivo de indice.
        gravarLog("Chave <%d> promovida\n", promo->id);             // Escreve no arquivo de log a promoção da nova raiz.
    }
    if(valorRetorno != ERRO)                                        // Caso a inserção retorne um resultado diferente de erro
        gravarLog("Chave <%d> inserida com sucesso.\n", id);        // grava a mensagem de inserção bem sucedida no arquivo de log.
    fclose(indice);                                                 // Fecha o arquivo de indice.
    return valorRetorno;                                            // Retorna o resultado da inserirArv.
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
    if(RRN_atual == -1)         // Base da recursão:
    {                           // Caso a página não exista (Árvore vazia ou filho de uma folha).
        *promo = novaChave;     // A nova chave será a chave promovida.
        *RRN_filho = -1;        // O RRN do seu filho será -1" (não há filho).
        return PROMOCAO;        // Retorna que houve uma promoção.
    }
    else                                                                    // Caso não entre no if acima.
    {                                                                       //
        pagina atual;                                                       // Declara uma nova página.
        carregaPagina(&atual, RRN_atual, indice);                           // Carrega em "atual" a página indicada pelo "RRN_atual".
        int pos = buscaBinaria(atual.chaves, &novaChave, 0, atual.tam-1);   // Busca a posição onde a chave deveria estar nessa página.
        chave promoAux;                                                     // Declara parâmetros auxiliares para a recursão.
        int RRN_filhoAux;                                                   //
        int valorRetorno = inserirArv(atual.filhos[pos], novaChave, &promoAux, &RRN_filhoAux, indice); //Chama a recursão para o filho da página atual na posição
                                                                                                       // indicada por "pos" (retorno da buscaBinaria).
        if(valorRetorno == NAOPROMOCAO)                                     // Caso a recursão retorne uma inserção onde não houve promoção.
            return valorRetorno;                                            // Retorna o mesmo resultado para o nível acima da recursão.
        else if(atual.tam < ORDEM-1)
        {                                                                                   // Caso haja espaço na página atual para a nova chave.
            atualizaPagina(atual.chaves, atual.filhos, &atual.tam, promoAux, RRN_filhoAux); // Insere a nova chave nessa página.
            escrevePagina(atual, RRN_atual, indice);                                        // Escreve a página atualizada no arquivo de indice.
            return NAOPROMOCAO;                                                             // Retorna para a recursão que não houve promoção.
        }
        else
        {                                                                                    // Caso não haja espaço na página atual para nova chave.
            pagina novaPagina;                                                               // Declara a nova página que será criada pelo split
            if(split(promoAux, RRN_filhoAux, &atual, &novaPagina, promo, RRN_filho) == ERRO) // Chama a função que executa o split da página atual.
                return ERRO;                                                                 // Caso ocorra algum erro, retorna ERRO.
            gravarLog("Divisao de no - pagina %d\n", RRN_atual);                             // Escreve no arquivo de log a operação de divisão da página.
            gravarLog("Chave <%d> promovida\n", promo->id);                                  // Escreve no arquivo de log a operação de promoção da chave.
            escrevePagina(atual, RRN_atual, indice);                                         // Escreve a página antiga (atualizada) no arquivo de indice.
            escrevePagina(novaPagina, *RRN_filho, indice);                                   // Escreve a nova página criada no mesmo arquivo.
            return PROMOCAO;                                                                 // Retorna para a recursão que houve promoção.
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
