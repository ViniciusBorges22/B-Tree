#ifndef DADOS_H_INCLUDED
#define DADOS_H_INCLUDED

#define ERRO -1           //
#define NAOENCONTRADO -2  // Valores de retorno das funções em geral
#define ENCONTRADO -3     //

typedef struct
{
	int id;
	char titulo[50];
	char genero[50];
}
tRegistro;


// Função que executa uma checagem geral dos arquivos de dados e de indice.
// O argumento "f1" é referente à funcionalidade 1 do programa. Caso ele seja TRUE, esta função sempre chamará
// a função "atualizaArvore". Caso seja FALSE, essa função será chamada apenas se a flag "atualizado" estiver em 0.
int checagem(int f1);

//Função que insere o registro (formatado) no arquivo de dados.
void inserirArq(int id, char titulo[], char genero[], FILE* dados);

//Função que atualiza o cabeçalho do arquivo de dados. A flag "atualizado" indica se a Árvore-B está atualizada em relação ao arquivo de dados.
void escreveCabecalhoDados(char flag, FILE* dados);

//Função que verifica no cabeçalho do arquivo se a Árvore-B está atualizada em relação ao arquivo de dados.
int estaAtualizado(FILE* dados);

//Função que carrega um registro do arquivo e armazena seus dados no ponteiro "registro" para quem a chamou.
int carregaRegistro(tRegistro *registro, FILE* dados);

//Função que monta uma string com os dados do registro para ser escrita no arquivo.
char montarBuffer(int id, char titulo[], char genero[], char buffer[]);

char *parser(char *buffer, int *pos);

#endif // DADOS_H_INCLUDED
