#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arvore_b.h"

void tratamentoEntrada(char aux[], char buffer[], char limite1, char limite2, int espaco);

int main()
{
    if(checagem(FALSE) == ERRO)                                                         //
    {                                                                                   // Verifica, através da função checagem, a existência do arquivo de dados
        fprintf(stderr, "\nErro na checagem inicial dos arquivos de dados e indices."); // e a atualização do arquivo de índices (caso esteja desatualizado).
        getchar();                                                                      // Imprime uma mensagem caso ocorra algum erro durante o processo.
        return ERRO;                                                                    //
    }
    int menu = TRUE;                //Variável responsável pela repetição do menu.
    int id, opt;                    //Variáveis para receber entradas do usuário: id recebe o ID a ser inserido e opt recebe a opção do menu.
    char buffer[50];                //Buffer para armazenar as entradas do usuário.
    unsigned long byteoffset;       //Variável para armazenar o byteoffset retornado da função de busca, para ser gravado no arqiuvo Log.
    tRegistro registro;             //registro auxiliar utilizado para a busca, para onde são retornados o título e gênero da música buscada.

    while(menu){

        system("cls");        //Realiza a limpeza da tela
        printf("\n MENU\n");                                                   //
        printf("\n 1 - Criar arvore a partir do arquivo de dados.");          //
        printf("\n 2 - Inserir novas musicas.");                             //         EXIBICAO DO MENU
        printf("\n 3 - Busca por id.");                                     //
        printf("\n 4 - Impressao da arvore.");                             //
        printf("\n 0 - Sair\n");                                          //
        printf("\n Opcao: ");                                            //

        fgets(buffer, 50, stdin);                       //Leitura da opção digitada do Menu
        sscanf(buffer, "%d", &opt);                     //Formataçao da entrada
        char limitante2 = '0', limitante1 = '0';        //Declaracao dos auxiliares usados na funcao do tratamentoEntrada
        char auxid[50];                                 //String auxiliar do ID, ja que o tratamentoEntrada é feito com Strings

        switch(opt){        //Switch da escolha do menu

            case 1:                                                                 //Caso seja escolhida a opção 1, o arquivo de indíces é criado a partir de um arquivo de dados já pronto
                if(checagem(TRUE) == ERRO)                                          //Executa a funcão checagem e faz uma verificação de erro. Caso ocorra um erro, exibe uma mensagem na tela.
                {
                    fprintf(stderr, "\nErro na criacao da arvore. Pressione qualquer tecla para continuar...");
                    getchar();                                                      //Pausa o programa para leitura da mensagem de erro.
                }
                break;

            case 2:                                                                 //Caso seja escolhida a opção 2, uma inserção é realizada
                printf("\n Titulo: ");
                char titulo[50];                                                    //String para armazenar a entrada após os tratamentos de entrada
                limitante1 = '0';                                                   //primeiro alfanumérico
                limitante2 = 'z';                                                   //último alfanumérico
                tratamentoEntrada(titulo, buffer, limitante1, limitante2, TRUE);    //Execucao da função de tratamento de entrada

                printf(" Genero: ");                                              //Nova informação a ser lida
                char genero[50];                                                    //String para armazenar a entrada após os tratamentos de entrada
                tratamentoEntrada(genero, buffer, limitante1, limitante2, TRUE);    //Execucao da função de tratamento de entrada

                printf(" ID: ");

                limitante1 = '0'; //numero 0.
                limitante2 = '9'; //numero 9.
                tratamentoEntrada(auxid, buffer, limitante1, limitante2, FALSE);     //Execucao da função de tratamento de entrada.
                id = atoi(auxid);                                                    //Converte o auxid do tipo char para tipo inteiro, ja que tratamentoEntrada trabalha apenas com strings.
                int retornoInsere = inserir(id, titulo, genero);                     //Chamada da função inserir e armazenamento do seu valor de retorno na variável retornoInsere.
                if(retornoInsere == ERRO)                                            //Verificação de erro de execução da funcao inserir.
                {
                    fprintf(stderr, "\nErro na insercao do arquivo. Pressione qualquer tecla para continuar...");
                    getchar();                                                       //Pausa no programa para leitura da mensagem de erro.
                }
                break;

            case 3:                                                                  //Caso seja escolhida a opção 3, uma busca pelo ID solicitado é realizada na árvore
                printf("\n ID: ");

                limitante1 = '0'; //Numero 0.
                limitante2 = '9'; //Numero 9.
                tratamentoEntrada(auxid, buffer, limitante1, limitante2, FALSE);     //Execucao da função de tratamento de entrada.
                id = atoi(auxid);                                                    //Converte o auxid do tipo char para tipo inteiro, ja que tratamentoEntrada trabalha apenas com strings.

                gravarLog("Execucao de operacao de PESQUISA de <%d>\n", id);         //Execução da função gravarLog, que grava no arquivo Log a requisição da operacao de pesquisa.

                if(busca(&registro, id, &byteoffset) == ENCONTRADO)                  //Verificação para saber se a chave solicitada foi encontrada.
                    gravarLog("Chave <%d> encontrada, offset <%li>, Titulo: <%s>, Genero: <%s>\n", id, byteoffset, registro.titulo, registro.genero); //grava no arquivo Log que a chave foi encontrada.
                else
                    gravarLog("Chave <%d> nao encontrada\n", id); //grava no arquivo Log que a chave nao foi encontrada.
                break;

            case 4:                                                                 //Caso seja escolhida a opção 4, executa a impressão da árvore.
                if(imprimeArvore() == ERRO)                                         //Executa a função de impressão da árvore no arquivo Log e verifica a existência de erro.
                {
                    fprintf(stderr, "\nErro na impressao da arvore. Pressione qualquer tecla para continuar...");
                    getchar();                                                      //Pausa no programa para leitura da mensagem de erro.
                }
                break;


            case 0:                 //Caso seja escolhida a opção 0, o programa é finalizado.
                menu = FALSE;       //Coloca a variável do laço para falso, saindo do loop de exibição do menu.
                break;

            default:                //Opcao default: nanhuma operação é realizada.
                break;
        }
    }

    return 0; //Fim do programa: retorna 0.
}

/*
*       Função que realiza o tratamento das entradas.
*       Funcionamento:  Dada a entrada, salva em uma string auxiliar, percorre toda a string aux checando se os caracteres
*                       estão dentro do intervalo de código ASCII especificado. Se todos os caracteres estiverem, aux é retornado
*                       e a entrada está correta. Caso algum dos caracteres estiver fora do intervalo delimitado, é requisitada
*                       uma nova entrada do usuário, e o processo é repetido.
*       ARGUMENTOS:
*                  char aux[] = string que ser� salva o conteudo da leitura.
*                  char buffer[] = auxiliar para leitura
*                  int limite1 e limite2 = Bordas menor e maior do intervalo de c�digo ASCII desejado.
*                  int espaco = serve para saber se o caractere " " (espaco) está habilitado na entrada
*
*/
void tratamentoEntrada(char aux[], char buffer[], char limite1, char limite2, int espaco){
    int condicional = TRUE;       //Variável para repeticao do laço.
    int erro;                     //Variável que indica se o usuário digitou a entrada corretamente.
    int cont;                     //Variável utilizada para o percorrimento da string inteira.
    while(condicional){           //Início do laço. Se repete até o usuário digitar uma entrada no formato correto, ou seja, sem caracteres especiais,
                                  //com ou sem espaco e dentro do intervalo definido no argumento.
        erro = FALSE;             //Inicialmente erro é setado para FALSO.
        cont = 0;                 //Contador inicia na primeira posicao da string (0).
        fgets(buffer, 50, stdin);       //Leitura da entrada com tamanho máximo de 50 caracteres.
        int tam = strlen(buffer) ;      //Obtencao do tamanho da String para podermos remover o new line "\n" que sempre é lido uma posicao
                                        //antes do final da string "\0".
        if (buffer[tam-1] == '\n')      //Verifica se realmente existe um \n em tam-1 (tam = tamanho da String)
            buffer[tam-1] = '\0';       //Substitui o new line "\n", se houver, por um final de string "\0", para que possamos gravar no arquivo sem quebra de linha.
        strcpy(aux, buffer);            //Copia a entrada do usuario, parcialmente formatada, para a string de retorno.

        while(aux[cont] != '\0' && erro == FALSE){                   // Laço interno: se repete enquanto nao chegarmos ao final da string e enquanto nao houver erro.
                                                                     //
            if( !(aux[cont] >= limite1 && aux[cont] <= limite2) ){   // Verifica se o caractere da string na posição cont não está dentro do intervalo.
                if(espaco){                                          // Se não esiver no intervalo, verifica-se se o espaço está permitido nesse formato de
                                                                     // entrada(título e gênero).(espaco é um argumento da funcao).
                    if(aux[cont] != 32){                             // Se espaco estiver habilitado e o caractere analisado nao for um espaço em branco " " (32 é o código ASCII
                                                                     // de espaco), a variável erro é setada como TRUE.
                        erro = TRUE;                                 // Se isso ocorrer o laço nao se repetirá mais, pois um caractere diferente do estabelecido
                    }                                                // foi inserido.
                }                                                    //
                else                                                 // Se o espaco nao estiver habilitado e o caractere estiver fora do intervalo estabelecido
                    erro = TRUE;                                     // entao é um caractere invalido, e a variável erro é setada como TRUE.
            }                                                        //
            cont++;                                                  // O contador é incrementado.
        }                                                            //

        if(erro == FALSE){      //Verifica se erro é falso. Caso seja, a variavel do primeiro while é setada para falso e a entrada está formatada corretamente.
            condicional = FALSE;
        }
        else                    //Se erro for true, ou seja, a entrada foi inserida incorretamente, printa para o usuário digitar novamente a entrada.
            printf("\n Formato de entrada incorreto. Digite novamente: ");
    }
    return; //Fim da função.
}
