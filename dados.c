#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dados.h"
#include "arvore_b.h"

// Função que executa uma checagem geral dos arquivos de dados e de indice.
// O argumento "f1" é referente à funcionalidade 1 do programa. Caso ele seja TRUE, esta função sempre chamará
// a função "atualizaArvore". Caso seja FALSE, essa função será chamada apenas se a flag "atualizado" estiver em 0.
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
            return ERRO;                                              // Caso ocorra algum erro, retorna ERRO.
        if(!estaAtualizado(dados) || f1)                              // Em seguida, verifica no cabeçalho se a Árvore-B não está atualizada ou se esta função
        {                                                             // foi chamada pela funcionalidade 1 do programa (f1 == TRUE).
            if(atualizaArvore() == ERRO)                              // Caso uma das condições seja verdade, chama a função que atualiza a Árvore.
                return ERRO;    //código de erro                      //
        }                                                             //
    }
    fclose(dados);                                                    // Fecha o arquivo de dados.
    return TRUE;                                                      // Caso não ocorra nenhum erro, retorna TRUE.
}

//Função que insere o registro (formatado) no arquivo de dados.
void inserirArq(int id, char titulo[], char genero[], FILE* dados)
{
    char buffer[200];                                       //Vetor onde será armazenada a string correspondente ao registro.
    char tam = montarBuffer(id, titulo, genero, buffer);    //Chama a função que monta a string a partir dos dados do registro.
    fwrite(&tam, sizeof(char), 1, dados);                   //Escreve o tamanho do registro no arquivo.
    fwrite(&buffer, tam, 1, dados);                         //Escreve o registro logo em seguida no arquivo.
    escreveCabecalhoDados(0, dados);                        //Atualiza o cabeçalho com a flag 0 (a Árvore-B não está atualizada).
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

//Função que carrega um registro do arquivo e armazena seus dados no ponteiro "registro" para quem a chamou.
int carregaRegistro(tRegistro *registro, FILE* dados)
{
    char size;                                           //tamanho do registro
    if(!fread(&size, sizeof(char), 1, dados))            //lê o tamanho do registro do arquivo e checa se nao chegou em EOF
        return ERRO;
    char buffer[200];
    fread(&buffer, size, 1, dados);                      //lê o registro e armazena no buffer
    int pos = 0;
    sscanf(parser(buffer, &pos), "%d", &(registro->id)); // Lê o primeiro campo do string correspondente ao ID da música.
    strcpy(registro->titulo, parser(buffer, &pos));      // Lê o segundo campo correspondente ao titulo da música.
    strcpy(registro->genero, parser(buffer, &pos));      // Lê o terceiro campo correspondente ao gênero da música.
    return ENCONTRADO;                                   // Retorna "registro encontrado".
}

//Função que monta uma string com os dados do registro para ser escrita no arquivo.
char montarBuffer(int id, char titulo[], char genero[], char buffer[])
{
    sprintf(buffer, "%d|%s|%s|", id, titulo, genero);   //Formata o buffer separando os campos do registro por barras verticais.
    return (char) strlen(buffer);                       //Retorna o tamanho resultante do registro.
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
