#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arvore_b.h"

void tratamentoEntrada(char aux[], char buffer[], char limite1, char limite2, int espaco);

int main()
{
    if(checagem(FALSE) == ERRO)                                                         //
    {                                                                                   // Chama a função que faz a checagem da existência do arquivo de dados
        fprintf(stderr, "\nErro na checagem inicial dos arquivos de dados e indices."); // e a atualização do arquivo de índices (caso esteja desatualizado).
        getchar();                                                                      // Imprime uma mensagem caso ocorra algum erro durante o processo.
        return ERRO;                                                                    //
    }
    int menu = TRUE;                //Variável responsável pela repeticao do menu
    int id, opt;                    //Usadas como entradas do usuário, uma para definir a opcao do menu e outra buscar e inserir ID
    char buffer[50];                //Auxiliar para leitura das entradas
    unsigned long byteoffset;       //Retornado da funao de busca, para ser gravado no Log
    tRegistro registro;             //Auxiliar para busca, onde são retornados o Titulo e genero

    while(menu){

        system("clear");        //Limpeza de tela
        printf("\n MENU\n");                                                   //
        printf("\n 1 - Criar arvore a partir do arquivo de dados.");          //
        printf("\n 2 - Inserir novas musicas.");                             //         EXIBICAO DO MENU
        printf("\n 3 - Busca por id.");                                     //
        printf("\n 4 - Impressao da arvore.");                             //
        printf("\n 0 - Sair\n");                                          //
        printf("\n Opcao: ");                                            //

        fgets(buffer, 50, stdin);           //Leitura da opcao digitada do Menu
        sscanf(buffer, "%d", &opt);         //Formataçao da entrada
        char limitante2 = '0', limitante1 = '0';    //Declaracao dos auxiliares usados na funcao do tratamentoEntrada
        char auxid[50];     //String Auxiliar do ID, ja que o tratamentoEntrada é feito com Strings

        switch(opt){        //Switch da escolha do menu

            case 1:     //Criacao do arquivo de indices a partir de um arquivo de dados ja pronto
                if(checagem(TRUE) == ERRO)      //Executa a funcao e faz checagem de erro
                {
                    fprintf(stderr, "\nErro na criacao da arvore. Pressione qualquer tecla para continuar...");     //Exibe na tela caso ocorra algum erro
                    getchar();              //Pausa no programa para mensagem ser lida
                }
                break;

            case 2:             //Opcao de insercao
                printf("Qual o titulo da musica a ser inserida?\n");        //Informacao a ser lida
                char titulo[50];       //Onde sera retornada a String apos o tratamento de entrada
                limitante1 = '0';     // primeiro alpha numerico
                limitante2 = 'z';    // ultimo alphanumerico
                tratamentoEntrada(titulo, buffer, limitante1, limitante2, TRUE);    //Execucao do tratamento de entrada

                printf("Qual o genero da musica a ser inserida?\n");        //Nova informação a ser lida
                char genero[50];            //Onde sera retornada a String apos o tratamento de entrada
                tratamentoEntrada(genero, buffer, limitante1, limitante2, TRUE);     //Execucao do tratamento de entrada

                printf("Qual o id da musica a ser inserida?\n");        //Informacao a ser lida

                limitante1 = '0';            //numero 0.
                limitante2 = '9';            //numero 9
                tratamentoEntrada(auxid, buffer, limitante1, limitante2, FALSE);     //Execucao do tratamento de entrada
                id = atoi(auxid);       //Converte o auxid de string para int, ja que tratamentoEntrada trabalha apenas com strings.
                int retornoInsere = inserir(id, titulo, genero);  //Todas as informacoes tentam ser inseridas
                if(retornoInsere == ERRO)       //Checagem de Erro na funcao inserir
                {
                    fprintf(stderr, "\nErro na insercao do arquivo. Pressione qualquer tecla para continuar...");
                    getchar();          //Pausa no programa para leitura da mensagem
                }
                break;

            case 3:         //Opcao de busca na arvore pelo ID
                printf("Qual o id da musica a ser buscada?\n");  //Informacao a ser lida (ID)

                limitante1 = '0';            //Numero 0.
                limitante2 = '9';            //Numero 9
                tratamentoEntrada(auxid, buffer, limitante1, limitante2, FALSE);     //Execucao do tratamento de entrada
                id = atoi(auxid);        //Converte o auxid de string para int, ja que tratamentoEntrada trabalha apenas com strings.

                gravarLog("Execucao de operacao de PESQUISA de <%d>\n", id);    //grava no Log a requisicao da operacao de Pesquisa

                if(busca(&registro, id, &byteoffset) == ENCONTRADO)     //Checagem para saber se a chave foi encontrada
                    gravarLog("Chave <%d> encontrada, offset <%li>, Titulo: <%s>, Genero: <%s>\n", id, byteoffset, registro.titulo, registro.genero);//Grava no Log q Foiencontrada
                else        //só entrará aqui caso a entrada nao for encontrada
                    gravarLog("Chave <%d> nao encontrada\n", id);   //Grava no Log que o ID nao foi encontrado
                break;

            case 4:
                if(imprimeArvore() == ERRO)         //Grava a arvore no Log e checa se houve erro
                {
                    fprintf(stderr, "\nErro na impressao da arvore. Pressione qualquer tecla para continuar...");       //Printa na tela se houve erro
                    getchar();      //Pausa no programa para o usuario ler a mensagem exibida
                }
                break;


            case 0:     //Opcao para sair do programa
                menu = FALSE;       //Troca a variavel do laco para Falsa, fazendo com que nao seja mais executado
                break;

            default:        //Opcao default
                break;      //Nada é realizado
        }
    }

    return 0;       //Programa encerrado, retorna 0
}

/*
*       Fun��o que realiza o tratamento das entradas.
*       Funcionamento: L� a entrada, salva em uma string auxiliar, percorre toda a string aux checando se os caracteres
*                       est�o dentro do intervalo de codigo ASCII especificado, se todos os caracteres estiverem, aux � retornado
*                       e a entrada est� correta, caso algum dos caracteres estiver fora do intervalo delimitado, � requerida
*                       uma nova entrada do usu�rio, e o processo � repetido.
*       ARGUMENTOS:
*                  char aux[] = string que ser� salva o conteudo da leitura.
*                  char buffer[] = auxiliar para leitura
*                  int limite1 e limite2 = Bordas menor e maior do intervalo de c�digo ASCII desejado.
*                  int espaco = serve para saber se o caractere " " (espaco) está habilitado na entrada
*
*/
void tratamentoEntrada(char aux[], char buffer[], char limite1, char limite2, int espaco){
    int condicional = TRUE;       //Condicional serve para repeticao do laço
    int erro;                     //Indica se o usuario Digitou a entrada corretamente
    int cont;                     //Usado para o percorrimento da String inteira
    while(condicional){           //While se repete até o usuario digitar uma entrada no formato correto, ou seja sem caracteres especiais,
                                  //com ou sem espaco e dentro do intervalo definido no argumento
        erro = FALSE;             //Inicialmente Erro é setado para FALSO
        cont = 0;                 //Contador inicia na primeira posicao da String (0)
        fgets(buffer, 50, stdin);       //Leitura da entrada com tamanho maximo de 50 caracteres
        int tam = strlen(buffer) ;      //Obtencao do tamanho da String para podermos remover o new line (\n) que sempre é lido uma posicao
                                        //antes do final da String(\0)
        if (buffer[tam-1] == '\n')      // Checa se realmente existe um \n em tam-1 (tam = tamanho da String)
            buffer[tam-1] = '\0';       //Substitui esse \n, se houver, por um fim de string (\0), para que possamos gravar no arquivo sem quebra de linha
        strcpy(aux, buffer);            //Copiamos agora a entrada do usuario, parcialmente formatada para a String de retorno

        while(aux[cont] != '\0' && erro == FALSE){                   // Laço se repete enquanto nao chegarmos no final da String e enquanto nao huver erro
                                                                     //
            if( !(aux[cont] >= limite1 && aux[cont] <= limite2) ){   // Checa se o caractere da String na posicao Cont nao está dentro do intervalo
                if(espaco){                                          // Se nao esiver no intervalo, checa-se se o espaço está permitido nesse formato de
                                                                     //  entrada(Titulo e Genero)(espaço é um argumento da funcao)
                    if(aux[cont] != 32){                             // Se espaco tiver habilitado e o caractere analisado nao for Espaco (32 é o código ASCII
                                                                     //  de espaco) Erro eh setado como TRUE
                        erro = TRUE;                                 // Se isso ocorrer o while nao se repetirá mais, pois um caractere diferente do estabelecido
                    }                                                //  foi inserido
                }                                                    //
                else                                                 // Se o espaco nao tiver habilitado e o caractere estiver fora do intervalo estabelecido
                    erro = TRUE;                                     //  entao eh um caractere invalido, e Erro é setado como TRUE
            }                                                        //
            cont++;                                                  //  Contado é incrementado
        }                                                            //

        if(erro == FALSE){      //Checa Erro é falso, caso positivo, a variavel do primeiro while é posta como Falso e a entrada está formatada corretamente
            condicional = FALSE;
        }
        else                    //Entra aqui se erro for True, ou seja, a entrada foi inserida incorretamente
            printf("Formato de entrada incorreto. Digite novamente.\n\t\tEntrada: ");       //Printa para o usuario re-Digitar a entrada
    }
    return;     //Fim da funcao
}
