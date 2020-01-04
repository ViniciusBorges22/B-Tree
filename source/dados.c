#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dados.h>
#include <arvore_b.h>

// Função que executa uma checagem geral dos arquivos de dados e de indice.
// O argumento "f1" é referente à funcionalidade 1 do programa. Caso ele seja TRUE, esta função sempre chamará
// a função "atualizaArvore". Caso seja FALSE, essa função será chamada apenas se a flag "atualizado" estiver em 0.
int checagem(int f1)
{
	FILE* dados;
	if((dados = fopen(data_file, "r+b")) == NULL)
	{
		if((dados = fopen(data_file, "w+b")) == NULL)
		{
			fprintf(stderr, "Error: could not create data file\n");
			return ERRO;
		}
		escreveCabecalhoDados(1, dados);
		if(verificaIndice() == ERRO)
			return ERRO;
	}
	else
	{
		if(verificaIndice() == ERRO)
			return ERRO;
		if(!estaAtualizado(dados) || f1)
		{
			if(atualizaArvore() == ERRO)
				return ERRO;
		}
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
	escreveCabecalhoDados(0, dados);
}

//Função que atualiza o cabeçalho do arquivo de dados. A flag "atualizado" indica se a Árvore-B está atualizada em relação ao arquivo de dados.
void escreveCabecalhoDados(char atualizado, FILE* dados)
{
	rewind(dados);
	fwrite(&atualizado, sizeof(char), 1, dados);
}

//Função que verifica no cabeçalho do arquivo se a Árvore-B está atualizada em relação ao arquivo de dados.
int estaAtualizado(FILE* dados)
{
	rewind(dados);
	char atualizado;
	fread(&atualizado, sizeof(char), 1, dados);
	return (int) atualizado;
}

//Função que carrega um registro do arquivo e armazena seus dados no ponteiro "registro" para quem a chamou.
int carregaRegistro(tRegistro *registro, FILE* dados)
{
	char size;
	if(!fread(&size, sizeof(char), 1, dados))
		return ERRO;
	char buffer[200];
	fread(&buffer, size, 1, dados);
	int pos = 0;
	sscanf(parser(buffer, &pos), "%d", &(registro->id));
	strcpy(registro->titulo, parser(buffer, &pos));
	strcpy(registro->genero, parser(buffer, &pos));
	return ENCONTRADO;
}

//Função que monta uma string com os dados do registro para ser escrita no arquivo.
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
