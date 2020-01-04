#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <tad_fila.h>

const char* btree_file = "btree.idx";
const char* data_file = "data.dad";
const char* log_file = "log.txt";

// Função que busca uma chave na Árvore-B através de um ID passado como parâmetro.
// Em seguida, retorna o registro buscado e o byteoffset dele no arquivo de dados atrás de parâmetros.
int busca(tRegistro* registro, int id, unsigned long* byteoffset)
{
	FILE* indice;
	if((indice = fopen(btree_file, "rb")) == NULL)
	{
		fprintf(stderr, "Error: could not open index file\n");
		return ERRO;
	}
	int raiz;
	carregaRaiz(&raiz, indice);
	pagina atual;
	if(carregaPagina(&atual, raiz, indice) == NAOENCONTRADO)
	{
		fclose(indice);
		return NAOENCONTRADO;
	}
	chave buscaChave;
	buscaChave.id = id;
	if(buscaAux(atual, &buscaChave, indice) == NAOENCONTRADO)
	{
		fclose(indice);
		return NAOENCONTRADO;
	}
	fclose(indice);
	*byteoffset = buscaChave.byteoffset;

	FILE* dados;
	if((dados = fopen(data_file, "rb")) == NULL)
	{
		fprintf(stderr, "Error: could not open data file\n");
		return ERRO;
	}
	fseek(dados, buscaChave.byteoffset, SEEK_SET);
	if(carregaRegistro(registro, dados) == ERRO)
		return NAOENCONTRADO;
	fclose(dados);
	return ENCONTRADO;
}

// Função de busca auxiliar que recebe uma página e executa a busca de uma chave a partir dessa página.
int buscaAux(pagina atual, chave* buscaChave, FILE* indice)
{
	int pos;
	while((pos = buscaBinaria(atual.chaves, buscaChave, 0, atual.tam-1)) != ENCONTRADO)
	{
		if(carregaPagina(&atual, atual.filhos[pos], indice) == NAOENCONTRADO)
			return NAOENCONTRADO;
	}
	return pos;
}

// Função que executa uma busca binária em um vetor de chaves em busca de um ID.
// Retorna o byteoffset do registro correspondente através do ponteiro "novaChave".
int buscaBinaria(chave chaves[], chave* novaChave, int esq, int dir)
{
	if(esq == dir)
	{
		if(novaChave->id < chaves[esq].id)
			return esq;
		else if(novaChave->id > chaves[esq].id)
			return esq+1;
		else
		{
			novaChave->byteoffset = chaves[esq].byteoffset;
			return ENCONTRADO;
		}
	}

	int mid = (esq + dir)/2;

	if(novaChave->id < chaves[mid].id)
		return buscaBinaria(chaves, novaChave, esq, mid);

	else if(novaChave->id > chaves[mid].id)
		return buscaBinaria(chaves, novaChave, mid+1, dir);

	else
	{
		novaChave->byteoffset = chaves[mid].byteoffset;
		return ENCONTRADO;
	}
}

// Função que executa a inserção do registro no arquivo de dados e da sua chave correspondente no arquivo de indice.
int inserir(int id, char titulo[], char genero[])
{
	gravarLog("INSERTION operation of <%d>, <%s>, <%s>.\n", id, titulo, genero);
	tRegistro registro;
	unsigned long byteoffset;
	if(busca(&registro, id, &byteoffset) == ENCONTRADO)
	{
		gravarLog("Duplicate key <%d>.\n", id);
		return ENCONTRADO;
	}
	FILE* dados;
	if((dados = fopen(data_file, "r+b")) == NULL)
	{
		fprintf(stderr, "Error: could not open data file\n");
		return ERRO;
	}
	fseek(dados, 0, SEEK_END);
	byteoffset = ftell(dados);
	inserirArq(id, titulo, genero, dados);
	fflush(dados);
	int valorRetorno = inserirAux(id, byteoffset);
	escreveCabecalhoDados(1, dados);
	fclose(dados);
	return valorRetorno;
}

// Função auxiliar que executa a inserção da chave na Árvore-B.
// Foi necessário criá-la para que não houvesse conflito entre as opções 1 e 2 do main, pois a opção 1 deve inserir apenas na Árvore.
// Além disso, esta função atualiza a raiz da Árvore caso haja uma promoção da raiz anterior durante a execução da "inserirArv".
int inserirAux(int id, unsigned long byteoffset)
{
	chave novaChave;
	novaChave.id = id;
	novaChave.byteoffset = byteoffset;
	int raiz;
	FILE* indice;
	if((indice = fopen(btree_file, "r+b")) == NULL)
	{
		fprintf(stderr, "Error: could not open index file\n");
		return ERRO;
	}
	carregaRaiz(&raiz, indice);
	chave promo;
	int RRN_filho;
	int valorRetorno = inserirArv(raiz, novaChave, &promo, &RRN_filho, indice);
	if(valorRetorno == PROMOCAO)
	{
		pagina novaRaiz;
		novaRaiz.chaves[0] = promo;
		novaRaiz.filhos[0] = raiz;
		novaRaiz.filhos[1] = RRN_filho;
		novaRaiz.tam = 1;
		int ultimo_rrn;
		fseek(indice, sizeof(int), SEEK_SET);
		fread(&ultimo_rrn, sizeof(int), 1, indice);
		raiz = ultimo_rrn++;
		escreveCabecalho(raiz, ultimo_rrn, indice);
		escrevePagina(novaRaiz, raiz, indice);
	}
	if(valorRetorno != ERRO)
		gravarLog("Key <%d> successfully inserted.\n", id);
	fclose(indice);
	return valorRetorno;
}

/*
   Parâmetros da função 'inserirArv':

RRN_atual: RRN da página que está sendo analisada
novaChave: chave a ser inserida na árvore
promo: chave promovida
RRN_filho: RRN do filho direito da chave promovida
*/

// Função que insere recursivamente uma nova chave na Árvore-B.
int inserirArv(int RRN_atual, chave novaChave, chave* promo, int* RRN_filho, FILE* indice)
{
	if(RRN_atual == -1)
	{
		*promo = novaChave;
		*RRN_filho = -1;
		return PROMOCAO;
	}
	else
	{
		pagina atual;
		carregaPagina(&atual, RRN_atual, indice);
		int pos = buscaBinaria(atual.chaves, &novaChave, 0, atual.tam-1);
		chave promoAux;
		int RRN_filhoAux;
		int valorRetorno = inserirArv(atual.filhos[pos], novaChave, &promoAux, &RRN_filhoAux, indice);

		if(valorRetorno == NAOPROMOCAO)
			return valorRetorno;
		else if(atual.tam < ORDEM-1)
		{
			atualizaPagina(atual.chaves, atual.filhos, &atual.tam, promoAux, RRN_filhoAux);
			escrevePagina(atual, RRN_atual, indice);
			return NAOPROMOCAO;
		}
		else
		{
			pagina novaPagina;
			if(split(promoAux, RRN_filhoAux, &atual, &novaPagina, promo, RRN_filho) == ERRO)
				return ERRO;
			gravarLog("Node division - page %d\n", RRN_atual);
			gravarLog("Key <%d> promoted\n", promo->id);
			escrevePagina(atual, RRN_atual, indice);
			escrevePagina(novaPagina, *RRN_filho, indice);
			return PROMOCAO;
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

// Função que executa um 'split' de uma página e divide as chaves entre as páginas "atual" e "novaPagina", com promoção da chave mediana.
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
	if((indice = fopen(btree_file, "r+b")) == NULL)
	{
		fprintf(stderr, "Error: could not open index file\n");
		return ERRO;
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

// Função que executa a impressão da Árvore-B no arquivo de log.
int imprimeArvore()
{
	gravarLog("Print B-Tree operation generated:\n");
	fila f;
	criarFila(&f);
	FILE* indice;
	if((indice = fopen(btree_file, "rb")) == NULL)
	{
		fprintf(stderr, "Error: could not open index file\n");
		return ERRO;
	}
	int raiz;
	carregaRaiz(&raiz, indice);
	pagina atual;
	if(carregaPagina(&atual, raiz, indice) == NAOENCONTRADO)
	{
		gravarLog("Error: B-Tree file not found\n");
		fclose(indice);
		return NAOENCONTRADO;
	}
	if(inserirFila(&f, atual) == ERRO)
	{
		fprintf(stderr, "Error: could not insert into queue\n");
		return ERRO;
	}
	int nivel = 0;
	int cont = 1;
	int somaCont = 0;
	while(!estaVaziaFila(&f))
	{
		if(cont == 0)
		{
			nivel++;
			cont = somaCont;
			somaCont = 0;
		}
		if(removerFila(&f, &atual) == ERRO)
		{
			fprintf(stderr, "Error: could not remove from queue\n");
			return ERRO;
		}
		imprimePagina(atual, nivel);
		cont--;
		int i;
		pagina aux;
		for(i = 0; i <= atual.tam; i++)
		{
			if(carregaPagina(&aux, atual.filhos[i], indice) == NAOENCONTRADO)
				break;
			if(inserirFila(&f, aux) == ERRO)
			{
				fprintf(stderr, "Error: could not insert on queue\n");
				return ERRO;
			}
		}
		somaCont += atual.tam + 1;
	}
	fclose(indice);
	return TRUE;
}

// Função auxiliar à "imprimeArvore".
// Recebe uma página e executa a impressão de seu nível, seu tamanho e suas chaves no arquivo de log.
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

// Função que lê a página do arquivo de indice a partir de um RRN passado como parâmetro.
int carregaPagina(pagina* atual, int RRN, FILE* indice)
{
	if(RRN < 0)
		return NAOENCONTRADO;
	fseek(indice, RRN*sizeof(pagina) + 2*sizeof(int), SEEK_SET);
	fread(atual, sizeof(pagina), 1, indice);
	return ENCONTRADO;
}

// Função que lê o RRN da raiz da Árvore no cabeçalho do arquivo de indice.
void carregaRaiz(int* raiz, FILE* indice)
{
	rewind(indice);
	fread(raiz, sizeof(int), 1, indice);
}

// Função que grava uma página no arquivo na posição indicada pelo RRN.
void escrevePagina(pagina atual, int RRN, FILE* indice)
{
	fseek(indice, RRN*sizeof(pagina) + 2*sizeof(int), SEEK_SET);
	fwrite(&atual, sizeof(pagina), 1, indice);
}

// Função que atualiza (ou escreve pela primeira vez) o cabeçalho do arquivo de indice.
// Argumentos:
//  raiz: RRN da raiz da Árvore-B.
//  ultimo_rrn: maior RRN da Árvore-B, utilizada para criar novas páginas no split e na atualização da raiz.
void escreveCabecalho(int raiz, int ultimo_rrn, FILE* indice)
{
	rewind(indice);
	fwrite(&raiz, sizeof(int), 1, indice);
	fwrite(&ultimo_rrn, sizeof(int), 1, indice);
}

// Função que grava no arquivo de log as operações executadas na Árvore-B.
// Para isso, ela recebe uma string ("mensagem") a ser formatada posteriormente pelos possíveis argumentos recebidos à direita dela, similar ao printf.
// Nesta função são utilizados os recursos da biblioteca stdarg.
int gravarLog(const char* mensagem, ...)
{
	FILE* log;
	if((log = fopen(log_file, "a")) == NULL)
	{
		fprintf(stderr, "Error: could not create or open log file\n");
		return ERRO;
	}
	va_list argumentos;
	va_start(argumentos, mensagem);
	vfprintf(log, mensagem, argumentos);
	va_end(argumentos);
	fclose(log);
	return TRUE;
}

// Função que executa uma checagem da existência do arquivo de indice.
// Caso não exista, cria o arquivo e inicializa o cabeçalho.
int verificaIndice()
{
	FILE* indice;
	if((indice = fopen(btree_file, "r+b")) == NULL)
	{
		if((indice = fopen(btree_file, "w+b")) == NULL)
		{
			fprintf(stderr, "Error: could not create index file\n");
			return ERRO;
		}
		int raiz = -1;
		int ultimo_rrn = 0;
		escreveCabecalho(raiz, ultimo_rrn, indice);
	}
	fclose(indice);
	return TRUE;
}

// Função usada para construir (ou atualizar, caso já exista) o arquivo de indices a partir de um arquivo de dados já inserido.
// Um laço percorre todo o arquivo de dados, armezenando os registros nele presente e os byteoffset de cada um deles
//  em cada ciclo desse laço o registro tenta ser inserido na arvore, o qual só será inserido se não estiver
//  presente nela. A condiçao de parada do laço é o fim do arquivo de dados.
int atualizaArvore()
{
	gravarLog("Creation of index '%s' from data file '%s'\n", btree_file, data_file);
	FILE* dados;
	if((dados = fopen(data_file, "rb")) == NULL){
		fprintf(stderr, "Error: could not read data file\n");
		return ERRO;
	}
	fseek(dados, sizeof(char), SEEK_SET);
	unsigned long byteoffset = ftell(dados);
	tRegistro registro;
	while(carregaRegistro(&registro, dados) != ERRO){
		if(busca(&registro, registro.id, &byteoffset) == ENCONTRADO)
		{
			byteoffset = ftell(dados);
			continue;
		}
		inserirAux(registro.id, byteoffset);
		byteoffset = ftell(dados);
	}
	fclose(dados);
	return TRUE;
}

// Função que insere uma nova chave em um vetor chaves na posição correta (seguindo a ordenação crescente).
void atualizaPagina(chave chaves[], int filhos[], unsigned short* tam, chave novaChave, int RRN_filho)
{
	int pos = buscaBinaria(chaves, &novaChave, 0, *tam-1);
	shiftDireita(chaves, filhos, pos, *tam);
	chaves[pos] = novaChave;
	filhos[pos+1] = RRN_filho;
	(*tam)++;
}

// Função que executa um shift a direita de todas as chaves posteriores a "inicio".
void shiftDireita(chave chaves[], int filhos[], int inicio, int tam)
{
	int i;
	for (i = tam; i > inicio; i--)
	{
		chaves[i] = chaves[i-1];
		filhos[i+1] = filhos[i];
	}
}
