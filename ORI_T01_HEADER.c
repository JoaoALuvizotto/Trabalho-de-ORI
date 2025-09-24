/* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Organização de Recuperação da Informação
 * Prof. Tiago A. Almeida
 *
 * Trabalho 01 - Indexação
 *
 * ========================================================================== *
 *   <<< IMPLEMENTE AQUI!!! COPIE E COLE O CONTEÚDO DESTE ARQUIVO NO AVA >>>
 * ========================================================================== */

// Bibliotecas
#include "ORI_T01_HEADER.h"
#include "utils.h"


// Defina aqui protótipos de funções auxiliares e macros (se quiser)
typedef struct {
	char id_treinador[TAM_ID_TREINADOR];
	char id_bolsomon[TAM_ID_BOLSOMON];
	int duracao;
	int eliminacoes;
	int dano;
} Dados_Treinador;

int qsort_int(const void *a, const void *b);
int encontrar_maior_duracao(Dados_Treinador dados_treinadores[], int n);
int encontrar_maior_eliminacoes(Dados_Treinador dados_treinadores[], int n);
int encontrar_maior_dano(Dados_Treinador dados_treinadores[], int n);


// Arquivos de dados
char ARQUIVO_TREINADORES[TAM_ARQUIVO_TREINADORES];
char ARQUIVO_BOLSOMONS[TAM_ARQUIVO_BOLSOMONS];
char ARQUIVO_BATALHAS[TAM_ARQUIVO_BATALHAS];
char ARQUIVO_RESULTADOS[TAM_ARQUIVO_RESULTADOS];
char ARQUIVO_TREINADOR_POSSUI_BOLSOMON[TAM_ARQUIVO_TREINADOR_POSSUI_BOLSOMON];


// Índices
treinadores_index *treinadores_idx = NULL;
bolsomons_index *bolsomons_idx = NULL;
batalhas_index *batalhas_idx = NULL;
resultados_index* resultados_idx = NULL;
treinador_possui_bolsomon_index *treinador_possui_bolsomon_idx = NULL;
preco_bolsomon_index *preco_bolsomon_idx = NULL;
data_index *data_idx = NULL;
inverted_list treinador_bolsomons_idx = {
        .treinador_bolsomons_secundario_idx = NULL,
        .treinador_bolsomons_primario_idx = NULL,
        .qtd_registros_secundario = 0,
        .qtd_registros_primario = 0,
        .tam_chave_secundaria = TAM_CHAVE_TREINADOR_BOLSOMON_SECUNDARIO_IDX,
        .tam_chave_primaria = TAM_CHAVE_TREINADOR_BOLSOMON_PRIMARIO_IDX,
};


// Contadores
unsigned qtd_registros_treinadores = 0;
unsigned qtd_registros_bolsomons = 0;
unsigned qtd_registros_batalhas = 0;
unsigned qtd_registros_resultados = 0;
unsigned qtd_registros_treinador_possui_bolsomon = 0;

// ---------------- Funções de comparação ----------------

/* Função de comparação entre chaves do índice treinadores_idx */
int qsort_treinadores_idx(const void *a, const void *b) {
	return strcmp(((treinadores_index*)a)->id_treinador, ((treinadores_index*)b)->id_treinador);
}

/* Função de comparação entre chaves do índice bolsomons_idx */
int qsort_bolsomons_idx(const void *a, const void *b) {
	return strcmp(((bolsomons_index*)a)->id_bolsomon, ((bolsomons_index*)b)->id_bolsomon);
}

/* Função de comparação entre chaves do índice batalhas_idx */
int qsort_batalhas_idx(const void *a, const void *b) {
	return strcmp(((batalhas_index*)a)->id_batalha, ((batalhas_index*)b)->id_batalha);
}

/* Função de comparação entre chaves do índice resultados_idx */
int qsort_resultados_idx(const void *a, const void *b) {
	if (strcmp(((resultados_index*)a)->id_treinador, ((resultados_index*)b)->id_treinador) == 0)
		return strcmp(((resultados_index*)a)->id_batalha, ((resultados_index*)b)->id_batalha);

	return strcmp(((resultados_index*)a)->id_treinador, ((resultados_index*)b)->id_treinador);
}

/* Função de comparação entre chaves do índice treinador_possui_bolsomon_idx */
int qsort_treinador_possui_bolsomon_idx(const void *a, const void *b) {
	if (strcmp(((treinador_possui_bolsomon_index*)a)->id_treinador, ((treinador_possui_bolsomon_index*)b)->id_treinador) == 0)
		return strcmp(((treinador_possui_bolsomon_index*)a)->id_bolsomon, ((treinador_possui_bolsomon_index*)b)->id_bolsomon);
	
	return strcmp(((treinador_possui_bolsomon_index*)a)->id_treinador, ((treinador_possui_bolsomon_index*)b)->id_treinador);
}

/* Função de comparação entre chaves do índice preco_bolsomon_idx */
int qsort_preco_bolsomon_idx(const void *a, const void *b){
	
	if (((preco_bolsomon_index*)a)->preco_bolsobolas == ((preco_bolsomon_index*)b)->preco_bolsobolas)
		return strcmp(((preco_bolsomon_index*)a)->id_bolsomon, ((preco_bolsomon_index*)b)->id_bolsomon);

	else if (((preco_bolsomon_index*)a)->preco_bolsobolas > ((preco_bolsomon_index*)b)->preco_bolsobolas)
		return 1;
	
	else 
		return -1;
}

/* Função usada na comparação entre as chaves do índice data_idx */
int qsort_data_idx(const void *a, const void *b) {
	if (strcmp(((data_index*)a)->inicio, ((data_index*)b)->inicio) == 0)
		return strcmp(((data_index*)a)->id_batalha, ((data_index*)b)->id_batalha);

	return strcmp(((data_index*)a)->inicio, ((data_index*)b)->inicio);
}

/* Função de comparação para a struct Info_Treinador, usada para determinar o campeão. */
int qsort_info_treinador(const void *a, const void *b) {
	const Info_Treinador *info_a = (Info_Treinador*)a;
	const Info_Treinador *info_b = (Info_Treinador*)b;

	if (info_b->vezes_mais_tempo != info_a->vezes_mais_tempo) 
		return info_b->vezes_mais_tempo - info_a->vezes_mais_tempo;
	 
	else if (info_b->vezes_mais_derrotados != info_a->vezes_mais_derrotados) 
		return info_b->vezes_mais_derrotados - info_a->vezes_mais_derrotados;
	
	else if (info_b->vezes_mais_dano != info_a->vezes_mais_dano) 
		return info_b->vezes_mais_dano - info_a->vezes_mais_dano;

	else 
		return strcmp(info_a->id_treinador, info_b->id_treinador);

}

/* Função de comparação entre chaves do índice secundário de treinador_bolsomons_secundario_idx */
int qsort_treinador_bolsomons_secundario_idx(const void *a, const void *b) {
	
	printf(ERRO_NAO_IMPLEMENTADO, "qsort_treinador_bolsomons_secundario_idx()");
	return 0;
}

/* Função de comparação entre chaves do índice primário da lista invertida */
int qsort_inverted_list_primary_search(const void *a, const void *b) {
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "qsort_inverted_list_primary_search()");
	return 0;
}

// ---------------- Criação do índice respectivo ----------------

void criar_treinadores_idx() {
	if(!treinadores_idx)
		treinadores_idx = malloc(MAX_REGISTROS * sizeof(treinadores_index));

	if(!treinadores_idx) {
		printf(ERRO_MEMORIA_INSUFICIENTE);
		exit(1);
	}

	for(unsigned i = 0; i < qtd_registros_treinadores; i++) {
		Treinador t = recuperar_registro_treinador(i);

		treinadores_idx[i].rrn = (strncmp(t.id_treinador, "*|", 2)) ? i : -1;

		strcpy(treinadores_idx[i].id_treinador, t.id_treinador);
	}

	qsort(treinadores_idx, qtd_registros_treinadores, sizeof(treinadores_index), qsort_treinadores_idx);
	printf(INDICE_CRIADO, "treinadores_idx");
}

void criar_bolsomons_idx() {
	if(!bolsomons_idx)
		bolsomons_idx = malloc(MAX_REGISTROS * sizeof(bolsomons_index));

	if(!bolsomons_idx) {
		printf(ERRO_MEMORIA_INSUFICIENTE);
		exit(1);
	}

	for(unsigned i = 0; i < qtd_registros_bolsomons; i++) {
		Bolsomon b = recuperar_registro_bolsomon(i);

		sprintf(bolsomons_idx[i].id_bolsomon, "%s", b.id_bolsomon);
		bolsomons_idx[i].rrn = i;
		
	}

	qsort(bolsomons_idx, qtd_registros_bolsomons, sizeof(bolsomons_index), qsort_bolsomons_idx);
	printf(INDICE_CRIADO, "bolsomons_idx");
}

void criar_batalhas_idx() {
	if(!batalhas_idx)
		batalhas_idx = malloc(MAX_REGISTROS * sizeof(batalhas_index));

	if(!batalhas_idx) {
		printf(ERRO_MEMORIA_INSUFICIENTE);
		exit(1);
	}
	for(unsigned i = 0; i < qtd_registros_batalhas; i++) {
		Batalha b = recuperar_registro_batalha(i);
			
		sprintf(batalhas_idx[i].id_batalha, "%s", b.id_batalha);
		batalhas_idx[i].rrn = i; 
	}

	qsort(batalhas_idx, qtd_registros_batalhas, sizeof(batalhas_index), qsort_batalhas_idx);
	printf(INDICE_CRIADO, "batalhas_idx");
}

void criar_resultados_idx(){
	if(!resultados_idx)
		resultados_idx = malloc(MAX_REGISTROS * sizeof(resultados_index));
	
	if(!resultados_idx) 
	{
		printf(ERRO_MEMORIA_INSUFICIENTE);
		exit(1);
	}
	for(unsigned i = 0; i < qtd_registros_resultados; i++) {
		Resultado r = recuperar_registro_resultado(i);
			
		sprintf(resultados_idx[i].id_treinador, "%s", r.id_treinador);
		sprintf(resultados_idx[i].id_batalha, "%s", r.id_batalha);
		resultados_idx[i].rrn = i;
	}

	qsort(resultados_idx, qtd_registros_resultados, sizeof(resultados_index), qsort_resultados_idx);
	printf(INDICE_CRIADO, "resultados_idx");
}

void criar_treinador_possui_bolsomon_idx(){
	if(!treinador_possui_bolsomon_idx)
		treinador_possui_bolsomon_idx = malloc(MAX_REGISTROS * sizeof(treinador_possui_bolsomon_index));

	if(!treinador_possui_bolsomon_idx) {
		printf(ERRO_MEMORIA_INSUFICIENTE);
		exit(1);
	}

	for(unsigned i = 0; i < qtd_registros_treinador_possui_bolsomon; i++) {
		TreinadorPossuiBolsomon tpb = recuperar_registro_treinador_possui_bolsomon(i);
			
		sprintf(treinador_possui_bolsomon_idx[i].id_treinador, "%s", tpb.id_treinador);
		sprintf(treinador_possui_bolsomon_idx[i].id_bolsomon, "%s", tpb.id_bolsomon);
		treinador_possui_bolsomon_idx[i].rrn = i;
	}
	
	qsort(treinador_possui_bolsomon_idx, qtd_registros_treinador_possui_bolsomon, sizeof(treinador_possui_bolsomon_index), qsort_treinador_possui_bolsomon_idx);
	printf(INDICE_CRIADO, "treinador_possui_bolsomon_idx");
}

void criar_preco_bolsomon_idx() {
	if(!preco_bolsomon_idx)
		preco_bolsomon_idx = malloc(MAX_REGISTROS * sizeof(bolsomons_index));

	if(!preco_bolsomon_idx) {
		printf(ERRO_MEMORIA_INSUFICIENTE);
		exit(1);
	}

	for(unsigned i = 0; i < qtd_registros_bolsomons; i++) {
		Bolsomon b = recuperar_registro_bolsomon(i);
			
		sprintf(preco_bolsomon_idx[i].id_bolsomon, "%s", b.id_bolsomon);
		preco_bolsomon_idx[i].preco_bolsobolas = b.preco_bolsobolas;
	}

	qsort(preco_bolsomon_idx, qtd_registros_bolsomons, sizeof(preco_bolsomon_index), qsort_preco_bolsomon_idx);

	printf(INDICE_CRIADO, "preco_bolsomon_idx");
}

void criar_data_idx() {
	if (!data_idx)
		data_idx = malloc(MAX_REGISTROS * sizeof(data_index));
		
	if (!data_idx) 
	{
		printf(ERRO_MEMORIA_INSUFICIENTE);
		exit(1);
	}


	for (unsigned i = 0; i < qtd_registros_batalhas; i++) 
	{
		Batalha b = recuperar_registro_batalha(i);
			
		sprintf(data_idx[i].inicio, "%s", b.inicio);
		sprintf(data_idx[i].id_batalha, "%s", b.id_batalha);
	}

	qsort(data_idx, qtd_registros_batalhas, sizeof(data_index), qsort_data_idx);
	printf(INDICE_CRIADO, "data_idx");

}

void criar_treinador_bolsomons_idx() {
	if (!treinador_bolsomons_idx.treinador_bolsomons_secundario_idx)
		treinador_bolsomons_idx.treinador_bolsomons_secundario_idx = malloc(MAX_REGISTROS * sizeof(treinador_bolsomons_secundario_index));
	
	if (!treinador_bolsomons_idx.treinador_bolsomons_secundario_idx)
	{
		printf(ERRO_MEMORIA_INSUFICIENTE);
		exit(1);
	}

	//Criar dois indices, treinador_bolsomons_primario_idx e treinador_bolsomons_secundario_idx
	//Percorrer o arquivo treinador_possui_bolsomon e preencher os dois indices
	
	
	

	
	printf(INDICE_CRIADO, "treinador_bolsomons_idx");
}

// ---------------- Recuperação do registro ----------------

Treinador recuperar_registro_treinador(int rrn) {
	Treinador t;
	char buffer[TAM_REGISTRO_TREINADOR + 1], *p;

	strncpy(buffer, ARQUIVO_TREINADORES + (rrn * TAM_REGISTRO_TREINADOR), TAM_REGISTRO_TREINADOR);
	buffer[TAM_REGISTRO_TREINADOR] = '\0';

	p = strtok(buffer, ";");
	strcpy(t.id_treinador, p);

	p = strtok(NULL, ";");
	strcpy(t.apelido, p);

	p = strtok(NULL, ";");
	strcpy(t.cadastro, p);

	p = strtok(NULL, ";");
	strcpy(t.premio, p);

	p = strtok(NULL, ";");
	t.bolsobolas = atof(p);

	return t;
}

Bolsomon recuperar_registro_bolsomon(int rrn) {
	Bolsomon b; 
	char buffer[TAM_REGISTRO_BOLSOMON + 1], *p;


	strncpy(buffer, ARQUIVO_BOLSOMONS + (rrn * TAM_REGISTRO_BOLSOMON), TAM_REGISTRO_BOLSOMON);
	buffer[TAM_REGISTRO_BOLSOMON] = '\0';
	
	p = strtok(buffer, ";");
	strcpy(b.id_bolsomon, p);

	p = strtok(NULL, ";");
	strcpy(b.nome, p);

	p = strtok(NULL, ";");
	strcpy(b.habilidade, p);

	p = strtok(NULL, ";");
	b.preco_bolsobolas = atof(p);

	return b;
}

Batalha recuperar_registro_batalha(int rrn) {
	Batalha b;
	int pos = rrn * TAM_REGISTRO_BATALHA;

    // Recuperando o ID da batalha
    strncpy(b.id_batalha, ARQUIVO_BATALHAS + pos, TAM_ID_BATALHA - 1);
    b.id_batalha[TAM_ID_BATALHA - 1] = '\0';
    pos += TAM_ID_BATALHA - 1;

    // Recuperando o inicio inicio
    strncpy(b.inicio, ARQUIVO_BATALHAS + pos, TAM_DATETIME - 1);
    b.inicio[TAM_DATETIME - 1] = '\0';
    pos += TAM_DATETIME - 1;

    // Recuperando a duracao duracao
    strncpy(b.duracao, ARQUIVO_BATALHAS + pos, TAM_TIME - 1);
    b.duracao[TAM_TIME - 1] = '\0';
    pos += TAM_TIME - 1;

    // Recuperando arena
    strncpy(b.arena, ARQUIVO_BATALHAS + pos, TAM_ARENA - 1);
    b.arena[TAM_ARENA - 1] = '\0';

    return b;
}

Resultado recuperar_registro_resultado(int rrn) {
	Resultado r;

	int pos = rrn * TAM_REGISTRO_RESULTADO;

    // Recuperando o ID do treinador
    strncpy(r.id_treinador, ARQUIVO_RESULTADOS + pos, TAM_ID_TREINADOR - 1);
    r.id_treinador[TAM_ID_TREINADOR - 1] = '\0';
    pos += TAM_ID_TREINADOR - 1;

    // Recuperando o ID da batalha
    strncpy(r.id_batalha, ARQUIVO_RESULTADOS + pos, TAM_ID_BATALHA - 1);
    r.id_batalha[TAM_ID_BATALHA - 1] = '\0';
    pos += TAM_ID_BATALHA - 1;

    // Recuperando o ID do bolsomon
    strncpy(r.id_bolsomon, ARQUIVO_RESULTADOS + pos, TAM_ID_BOLSOMON - 1);
    r.id_bolsomon[TAM_ID_BOLSOMON - 1] = '\0';
    pos += TAM_ID_BOLSOMON - 1;

    // Recuperando os atributos de ranking
    r.foi_maior_duracao = ARQUIVO_RESULTADOS[pos++];
    r.foi_mais_derrotas = ARQUIVO_RESULTADOS[pos++];
    r.foi_mais_dano = ARQUIVO_RESULTADOS[pos++];

    return r;
}

TreinadorPossuiBolsomon recuperar_registro_treinador_possui_bolsomon(int rrn) {
	TreinadorPossuiBolsomon tpb;
	
	char *p;
	p = ARQUIVO_TREINADOR_POSSUI_BOLSOMON + (rrn * TAM_REGISTRO_TREINADOR_POSSUI_BOLSOMON);

	snprintf(tpb.id_treinador, TAM_ID_TREINADOR, "%s", p);

	p = p + TAM_ID_TREINADOR - 1;

	snprintf(tpb.id_bolsomon, TAM_ID_BOLSOMON, "%s", p);
	
	
	return tpb;
}

// ---------------- Gravação nos arquivos de dados ----------------

void escrever_registro_treinador(Treinador t, int rrn) {
	char buffer[TAM_REGISTRO_TREINADOR + 1], double_str[100];
	buffer[0] = '\0';
	double_str[0] = '\0';

	strcat(buffer, t.id_treinador); strcat(buffer, ";");
	strcat(buffer, t.apelido); strcat(buffer, ";");
	strcat(buffer, t.cadastro); strcat(buffer, ";");
    strcat(buffer, t.premio); strcat(buffer, ";");
	sprintf(double_str, "%013.2lf", t.bolsobolas);
	strcat(buffer, double_str);	strcat(buffer, ";");
    
	strpadright(buffer, '#', TAM_REGISTRO_TREINADOR);
    
	strncpy(ARQUIVO_TREINADORES + rrn * TAM_REGISTRO_TREINADOR, buffer, TAM_REGISTRO_TREINADOR);
}

void escrever_registro_bolsomon(Bolsomon b, int rrn) {
	char buffer[TAM_REGISTRO_BOLSOMON + 1], double_str[100];
	buffer[0] = '\0';
	double_str[0] = '\0';

	strcat(buffer, b.id_bolsomon);
	strcat(buffer, ";");
	strcat(buffer, b.nome);
	strcat(buffer, ";");
	strcat(buffer, b.habilidade);
	strcat(buffer, ";");
	sprintf(double_str, "%013.2lf", b.preco_bolsobolas);
	strcat(buffer, double_str);
	strcat(buffer, ";");
    
	strpadright(buffer, '#', TAM_REGISTRO_BOLSOMON);
    
	strncpy(ARQUIVO_BOLSOMONS + rrn * TAM_REGISTRO_BOLSOMON, buffer, TAM_REGISTRO_BOLSOMON);
}

void escrever_registro_batalha(Batalha b, int rrn) {
	char buffer[TAM_REGISTRO_BATALHA + 1];
	buffer[0] = '\0';

	strcat(buffer, b.id_batalha); 
	strcat(buffer, b.inicio); 
	strcat(buffer, b.duracao); 
	strcat(buffer, b.arena); 
	
	strpadright(buffer, '#', TAM_REGISTRO_BATALHA);
	
	strncpy(ARQUIVO_BATALHAS + rrn * TAM_REGISTRO_BATALHA, buffer, TAM_REGISTRO_BATALHA);
}

void escrever_registro_resultado(Resultado r, int rrn) {
	char buffer[TAM_REGISTRO_RESULTADO + 1];
	buffer[0] = '\0';

	strcat(buffer, r.id_treinador); 
	strcat(buffer, r.id_batalha); 
	strcat(buffer, r.id_bolsomon);
	
	
	//Adicionar atributos de ranking e convertendo o booleanos para '1' ou '0'
 	sprintf(buffer + strlen(buffer), "%c", r.foi_maior_duracao ? '1' : '0');
    sprintf(buffer + strlen(buffer), "%c", r.foi_mais_derrotas ? '1' : '0');
    sprintf(buffer + strlen(buffer), "%c", r.foi_mais_dano ? '1' : '0');
	
	strpadright(buffer, '#', TAM_REGISTRO_RESULTADO);

	strncpy(ARQUIVO_RESULTADOS + rrn * TAM_REGISTRO_RESULTADO, buffer, TAM_REGISTRO_RESULTADO);
}

void escrever_registro_treinador_possui_bolsomon(TreinadorPossuiBolsomon tpb, int rrn) {
	char buffer[TAM_REGISTRO_TREINADOR_POSSUI_BOLSOMON + 1];
	buffer[0] = '\0';

	strcat(buffer, tpb.id_treinador); 
	strcat(buffer, tpb.id_bolsomon);
	
	strncpy(ARQUIVO_TREINADOR_POSSUI_BOLSOMON + rrn * TAM_REGISTRO_TREINADOR_POSSUI_BOLSOMON, buffer, TAM_REGISTRO_TREINADOR_POSSUI_BOLSOMON);
}

// ---------------- Exibição dos registros ----------------

bool exibir_treinador(int rrn) {
	if(rrn < 0) return false;
    
	Treinador t = recuperar_registro_treinador(rrn);
    
	printf("%s, %s, %s, %s, %.2lf\n", t.id_treinador, t.apelido, t.cadastro, t.premio, t.bolsobolas);
    
	return true;
}

bool exibir_bolsomon(int rrn) {
	if(rrn < 0) 
	return false;
    
	Bolsomon b = recuperar_registro_bolsomon(rrn);
    
	printf("%s, %s, %s, %.2lf\n", b.id_bolsomon, b.nome, b.habilidade, b.preco_bolsobolas);
    
	return true;
}

bool exibir_batalha(int rrn) {
	if(rrn < 0) 
	return false;
	
	Batalha b = recuperar_registro_batalha(rrn);
	
	printf("%s, %s, %s, %s\n", b.id_batalha, b.inicio, b.duracao, b.arena);
	
	return true;
}

// ---------------- Funções principais ----------------

void cadastrar_treinador_menu(char *id_treinador, char *apelido) {
	int retorno_busca = busca_binaria(id_treinador,  treinadores_idx, qtd_registros_treinadores, sizeof(treinadores_index), qsort_treinadores_idx, false, 0);
	//int retorno_busca = bsearch((void*)id_treinador, treinadores_idx, qtd_registros_treinadores, sizeof(treinadores_index), qsort_treinadores_idx) != NULL ? 1 : -1;

	if (retorno_busca >= 0)
	{
		printf(ERRO_PK_REPETIDA, id_treinador);
		return;
	}
	
	//Criando Treinador
	Treinador t;

	sprintf(t.id_treinador, "%s", id_treinador);
	sprintf(t.apelido, "%s", apelido);
	t.bolsobolas = 0.0;
	current_datetime(t.cadastro);
	strpadright(t.premio, '0', TAM_DATETIME -1);

	//Atualizando o indice 
	sprintf(treinadores_idx[qtd_registros_treinadores].id_treinador, "%s", id_treinador);
	treinadores_idx[qtd_registros_treinadores].rrn = qtd_registros_treinadores;
	
	qsort(treinadores_idx, qtd_registros_treinadores + 1, sizeof(treinadores_index), qsort_treinadores_idx);

	//Atualizando o arquivo de dados dos treinadores
	escrever_registro_treinador(t, qtd_registros_treinadores);

	//somando um para nos treinadores
	qtd_registros_treinadores++;


	printf(SUCESSO);
}

void remover_treinador_menu(char *id_treinador) {
	int retorno_busca = busca_binaria(id_treinador, treinadores_idx, qtd_registros_treinadores, sizeof(treinadores_index), qsort_treinadores_idx, false, 0);
	if (retorno_busca == -1)
	{
		printf(ERRO_REGISTRO_NAO_ENCONTRADO);
		return;
	}

	Treinador t = recuperar_registro_treinador(treinadores_idx[retorno_busca].rrn);
	
	//Marcando o registro como
	strncpy(t.id_treinador, "*|", 2);

	//Atualizando o arquivo de dados
	escrever_registro_treinador(t, treinadores_idx[retorno_busca].rrn);
	
	//Atualizando o indice
	// sprintf(treinadores_idx[retorno_busca].id_treinador, "%s", treinadores_idx[qtd_registros_treinadores - 1].id_treinador);
	// treinadores_idx[retorno_busca].rrn = treinadores_idx[qtd_registros_treinadores - 1].rrn;
	// treinadores_idx[qtd_registros_treinadores - 1].rrn = -1;
	// qtd_registros_treinadores--;
	// qsort(treinadores_idx, qtd_registros_treinadores, sizeof(treinadores_index), qsort_treinadores_idx);
	treinadores_idx[retorno_busca].rrn = -1;

	printf(SUCESSO);
}

void adicionar_bolsobolas_menu(char *id_treinador, double valor) {
	adicionar_bolsobolas(id_treinador, valor, true);
}

void adicionar_bolsobolas(char *id_treinador, double valor, bool flag){
	int retorno_busca = busca_binaria(id_treinador, treinadores_idx, qtd_registros_treinadores, sizeof(treinadores_index), qsort_treinadores_idx, false, 0);
	if (valor <= 0)
	{
		printf(ERRO_VALOR_INVALIDO);
		return;
	}
	
	if (retorno_busca == -1 || treinadores_idx[retorno_busca].rrn == -1)
	{
		printf(ERRO_REGISTRO_NAO_ENCONTRADO);
		return;
	}

	Treinador t = recuperar_registro_treinador(treinadores_idx[retorno_busca].rrn);
	t.bolsobolas += valor;
	escrever_registro_treinador(t, treinadores_idx[retorno_busca].rrn);

	//Atualizar o arquivo de dados
	//Encontrando o local onde o registro do treinador está salvo no arquivo
	char *p = ARQUIVO_TREINADORES + treinadores_idx[retorno_busca].rrn * TAM_REGISTRO_TREINADOR;

	//Pos é qualquer caracter do registro diferente de ;
    int cont = 0, pos = 0;

    //Acessando campo saldo no registro
    while (cont < 4) 
	{
		//Encontrando onde que está o campo das bolsobolas no registro do treinador 
		//Decidi contar a quantidade de ;
        if (p[pos] == ';') 
            cont++;

        pos++;
    }

    //Alterando saldo
	//Achando a posição do campo saldo das bolsobolas
    p = p + pos;

	//Criando uma string para armazenar o novo saldo
	char novo_saldo[TAM_FLOAT_NUMBER];

	//Atualizando o saldo no arquivo
	sprintf(novo_saldo, "%013.2lf", t.bolsobolas);
    strncpy(p, novo_saldo, TAM_FLOAT_NUMBER - 1);
	
	printf(SUCESSO);
}

void cadastrar_bolsomon_menu(char *nome, char *habilidade, double preco) {
	int retorno_busca = busca_binaria(nome,  bolsomons_idx, qtd_registros_bolsomons, sizeof(bolsomons_index), qsort_bolsomons_idx, false, 0);
	if (retorno_busca >= 0)
	{
		printf(ERRO_PK_REPETIDA, nome);
		return;
	}

	for (unsigned i = 0; i < qtd_registros_bolsomons; i++)
	{
		Bolsomon b = recuperar_registro_bolsomon(i);
		if (strcmp(b.nome, nome) == 0)
		{
			retorno_busca = i;
			break;
		}
	}

	if (retorno_busca == 1)
	{
		printf(ERRO_PK_REPETIDA, nome);
		return;
	}

	Bolsomon b;
	sprintf(b.id_bolsomon, "%03d", qtd_registros_bolsomons);
	sprintf(b.nome, "%s", nome);
	sprintf(b.habilidade, "%s", habilidade);
	b.preco_bolsobolas = preco;

	//Atualizando o indice bolsomon_idx
	sprintf(bolsomons_idx[qtd_registros_bolsomons].id_bolsomon, "%03d", qtd_registros_bolsomons);
	bolsomons_idx[qtd_registros_bolsomons].rrn = qtd_registros_bolsomons;
	qsort(bolsomons_idx, qtd_registros_bolsomons + 1, sizeof(bolsomons_index), qsort_bolsomons_idx);

	//Atualizando o indice bolsomon_preco_idx
	sprintf(preco_bolsomon_idx[qtd_registros_bolsomons].id_bolsomon, "%03d", qtd_registros_bolsomons);
	preco_bolsomon_idx[qtd_registros_bolsomons].preco_bolsobolas = preco;
	qsort(preco_bolsomon_idx, qtd_registros_bolsomons + 1, sizeof(preco_bolsomon_index), qsort_preco_bolsomon_idx);

	//Atualizando o arquivo de dados dos bolsomons
	escrever_registro_bolsomon(b, qtd_registros_bolsomons);
	//somando um para nos bolsomons
	qtd_registros_bolsomons++;

	printf(SUCESSO);
}

void comprar_bolsomon_menu(char *id_treinador, char *id_bolsomon) {
	//Verificar se treinador existe
	int retorno_busca_treinador = busca_binaria(id_treinador, treinadores_idx, qtd_registros_treinadores, sizeof(treinadores_index), qsort_treinadores_idx, false, 0);
	if (retorno_busca_treinador == -1)
	{
		printf(ERRO_REGISTRO_NAO_ENCONTRADO);
		return;
	}

	//Verificar se bolsomon existe
	int retorno_busca_bolsomon = busca_binaria(id_bolsomon, bolsomons_idx, qtd_registros_bolsomons, sizeof(bolsomons_index), qsort_bolsomons_idx, false, 0);
	if (retorno_busca_bolsomon == -1)
	{
		printf(ERRO_REGISTRO_NAO_ENCONTRADO);
		return;
	}

	//Recuperar registros
	Treinador t = recuperar_registro_treinador(treinadores_idx[retorno_busca_treinador].rrn);
	Bolsomon b = recuperar_registro_bolsomon(bolsomons_idx[retorno_busca_bolsomon].rrn);

	//Verificar se treinador tem bolsobolas suficiente
	if (t.bolsobolas < b.preco_bolsobolas)
	{
		printf(ERRO_SALDO_NAO_SUFICIENTE);
		return;
	}

	//Atualizar o saldo do treinador
	t.bolsobolas -= b.preco_bolsobolas;
	
	//Atualizar o arquivo de dados do treinador
	escrever_registro_treinador(t, treinadores_idx[retorno_busca_treinador].rrn);

	//Atualizar índice treinador_possui_bolsomon_idx
	sprintf(treinador_possui_bolsomon_idx[qtd_registros_treinador_possui_bolsomon].id_treinador, "%s", id_treinador);
	sprintf(treinador_possui_bolsomon_idx[qtd_registros_treinador_possui_bolsomon].id_bolsomon, "%s", id_bolsomon);
	treinador_possui_bolsomon_idx[qtd_registros_treinador_possui_bolsomon].rrn = qtd_registros_treinador_possui_bolsomon;
	qsort(treinador_possui_bolsomon_idx, qtd_registros_treinador_possui_bolsomon + 1, sizeof(treinador_possui_bolsomon_index), qsort_treinador_possui_bolsomon_idx);

	//Atualizar arquivo de dados treinador_possui_bolsomon
	TreinadorPossuiBolsomon tpb;
	sprintf(tpb.id_treinador, "%s", id_treinador);
	sprintf(tpb.id_bolsomon, "%s", id_bolsomon);

	escrever_registro_treinador_possui_bolsomon(tpb, qtd_registros_treinador_possui_bolsomon);

	qtd_registros_treinador_possui_bolsomon++;

	printf(SUCESSO);
}

void executar_batalha_menu(char *inicio, char *duracao, char *arena, char *id_treinadores, char *bolsomons_treinadores, char *duracoes_treinadores, char *eliminacoes_treinadores, char *danos_causados_treinadores) {
	//Verificando se cada treinador existe
	char token_treinador[TAM_ID_TREINADOR];
	char token_bolsomon[TAM_ID_BOLSOMON];
	char *p, *q;

	p = id_treinadores;
	q = bolsomons_treinadores;

	strncpy(token_treinador, p, TAM_ID_TREINADOR - 1);
	token_treinador[TAM_ID_TREINADOR - 1] = '\0';

	strncpy(token_bolsomon, q, TAM_ID_BOLSOMON - 1);
	token_bolsomon[TAM_ID_BOLSOMON - 1] = '\0';
	

	//Verificando se todos os treinadores existem e se cada um possui o respectivo o bolsomon
	
	for (int cont = 0; cont < 12; cont++)
	{
		int retorno_busca_treinador = busca_binaria(token_treinador, treinadores_idx, qtd_registros_treinadores, sizeof(treinadores_index), qsort_treinadores_idx, false, 0);
		if (retorno_busca_treinador == -1)
		{
			printf(ERRO_REGISTRO_NAO_ENCONTRADO);
			return;
		}
			
		//Verificando se treinador possui bolsomon
		int i = 0;
		while (strcmp(treinador_possui_bolsomon_idx[i].id_treinador, token_treinador) < 0 && i < qtd_registros_treinador_possui_bolsomon)
			i++;
		
		//Caso em que treinador não possui nenhum bolsomon (não há registro do treinador no índice treinador_possui_bolsomon)
		if (i > qtd_registros_treinador_possui_bolsomon) {
			//Consultar o índice de bolsomons para recuperar o rrn
			int retorno_busca_bolsomon = busca_binaria(token_bolsomon, bolsomons_idx, qtd_registros_bolsomons, sizeof(bolsomons_index), qsort_bolsomons_idx, false, 0);
			Bolsomon bolsomon = recuperar_registro_bolsomon(bolsomons_idx[retorno_busca_bolsomon].rrn);

			printf(ERRO_TREINADOR_BOLSOMON, token_treinador, bolsomon.nome);
			return;
		}

		//Procurando o bolsomon na lista de bolsomons do treinador
		bool encontrou = false;
		while (strcmp(treinador_possui_bolsomon_idx[i].id_treinador, token_treinador) == 0) 
		{
			if (strcmp(treinador_possui_bolsomon_idx[i].id_bolsomon, token_bolsomon) == 0) {
				encontrou = true;
				break;
			}		
			i++;

		}
		//Treinador não possui o bolsomon
		if (!encontrou) {
			int retorno_busca_bolsomon = busca_binaria(token_bolsomon, bolsomons_idx, qtd_registros_bolsomons, sizeof(bolsomons_index), qsort_bolsomons_idx, false, 0);
			Bolsomon bolsomon = recuperar_registro_bolsomon(bolsomons_idx[retorno_busca_bolsomon].rrn);

			printf(ERRO_TREINADOR_BOLSOMON, token_treinador, bolsomon.nome);
			return;
		}

		//Pegando o próximo treinador e bolsomon
		p = p + TAM_ID_TREINADOR - 1;
		strncpy(token_treinador, p, TAM_ID_TREINADOR - 1);
		token_treinador[TAM_ID_TREINADOR - 1] = '\0';
		
		q = q + TAM_ID_BOLSOMON - 1;
		strncpy(token_bolsomon, q, TAM_ID_BOLSOMON - 1);
		token_bolsomon[TAM_ID_BOLSOMON - 1] = '\0';

	}

	//Criando struct Batalha
	Batalha b;
	sprintf(b.id_batalha, "%08u", qtd_registros_batalhas);
	sprintf(b.inicio, "%s", inicio);	
	sprintf(b.duracao, "%s", duracao);
	sprintf(b.arena, "%s", arena);

	//Atualizando arquivo de batalhas
	escrever_registro_batalha(b, qtd_registros_batalhas);

	//Atualizando índice batalhas_idx
	sprintf(batalhas_idx[qtd_registros_batalhas].id_batalha, "%08u", qtd_registros_batalhas);
	batalhas_idx[qtd_registros_batalhas].rrn = qtd_registros_batalhas;
	qsort(batalhas_idx, qtd_registros_batalhas + 1, sizeof(batalhas_index), qsort_batalhas_idx);

	qtd_registros_batalhas++;


	//Populando os dados dos treinadores
	Dados_Treinador dados_treinadores[12];

	for (int i = 0; i < 12; i++)
	{
		//ID do treinador
		char p[TAM_ID_TREINADOR];
		strncpy(p, id_treinadores + i * (TAM_ID_TREINADOR - 1), TAM_ID_TREINADOR - 1);
		p[TAM_ID_TREINADOR - 1] = '\0';
		snprintf(dados_treinadores[i].id_treinador, TAM_ID_TREINADOR, "%s", p);

		//ID do bolsomon
		char q[TAM_ID_BOLSOMON];
		strncpy(q, bolsomons_treinadores + i * (TAM_ID_BOLSOMON -1), TAM_ID_BOLSOMON -1);
		q[TAM_ID_BOLSOMON - 1] = '\0';
		snprintf(dados_treinadores[i].id_bolsomon, TAM_ID_BOLSOMON, "%s", q);

		//Duracao
		char r[TAM_TIME];
		strncpy(r, duracoes_treinadores + i * (TAM_TIME -1), TAM_TIME - 1);
		r[TAM_TIME - 1] = '\0';	
		dados_treinadores[i].duracao = atoi(r);

		//Eliminacoes
		char s[5];
		strncpy(s, eliminacoes_treinadores + i * 4, 4);
		s[4] = '\0';
		dados_treinadores[i].eliminacoes = atoi(s);

		//Dano
		char t[5];
		strncpy(t, danos_causados_treinadores + i * 4, 4);
		t[4] = '\0';	
		dados_treinadores[i].dano = atoi(t);
	}

	//Encontrando os maiores
	int maior_duracao = encontrar_maior_duracao(dados_treinadores, 12);
	int maior_eliminacoes = encontrar_maior_eliminacoes(dados_treinadores, 12);
	int maior_dano = encontrar_maior_dano(dados_treinadores, 12);

	
	//Criando registros de resultados
	for (int i = 0; i < 12; i++) {
		Resultado r;
		sprintf(r.id_batalha, "%08u", qtd_registros_batalhas - 1);
		sprintf(r.id_treinador, "%s", dados_treinadores[i].id_treinador);
		sprintf(r.id_bolsomon, "%s", dados_treinadores[i].id_bolsomon);
		if (i == maior_duracao) {
			r.foi_maior_duracao = true;
		}
		else {
			r.foi_maior_duracao = false;
		}
		if (i == maior_eliminacoes) {
			r.foi_mais_derrotas = true;
		}
		else {
			r.foi_mais_derrotas = false;
		}
		if (i == maior_dano) {
			r.foi_mais_dano = true;
		}
		else {
			r.foi_mais_dano = false;
		}

		//Atualizando arquivo de resultados
		escrever_registro_resultado(r, qtd_registros_resultados);

		//Atualizando índice resultados_idx
		sprintf(resultados_idx[qtd_registros_resultados].id_batalha, "%08u", qtd_registros_batalhas - 1);
		sprintf(resultados_idx[qtd_registros_resultados].id_treinador, "%s", dados_treinadores[i].id_treinador);
		resultados_idx[qtd_registros_resultados].rrn = qtd_registros_resultados;
		qsort(resultados_idx, qtd_registros_resultados + 1, sizeof(resultados_index), qsort_resultados_idx);

		qtd_registros_resultados++;
	}
	
	printf(SUCESSO);
}

void recompensar_campeao_menu(char *data_inicio, char *data_fim, double premio) {
	int inicio = busca_binaria(data_inicio, data_idx, qtd_registros_batalhas, sizeof(data_index), qsort_data_idx, true, 1);
	int fim = busca_binaria(data_fim, data_idx, qtd_registros_batalhas, sizeof(data_index), qsort_data_idx, true, -1);
	
	if (inicio == -1 || fim == -1 || inicio > fim) {
        printf(AVISO_NENHUM_REGISTRO_ENCONTRADO);
        return;
    }

	//Tenhos os os índices do primeiro registro e do último registro
	while (inicio > 0 && strcmp(data_idx[inicio - 1].inicio, data_idx[inicio].inicio) == 0) 
    	inicio--;
	
	while (fim < (qtd_registros_batalhas - 1) && strcmp(data_idx[fim + 1].inicio, data_idx[fim].inicio) == 0)
    	fim++;

	

	Info_Treinador campeoes[qtd_registros_treinadores];
	int iterador_campeoes = 0;
	//Percorrendo os registros de batalhas no período
	//E para cada batalha, percorrendo os resultados para encontrar o campeão
	for (int i = inicio; i <= fim; i++)
	{
		char id_batalha_atual[TAM_ID_BATALHA];
    	strcpy(id_batalha_atual, data_idx[i].id_batalha);

    	for (int j = 0; j < qtd_registros_resultados; j++) 
		{ // Para cada resultado existente
        	Resultado r = recuperar_registro_resultado(j);
			
			// Se o resultado pertence à batalha atual
			if (strcmp(r.id_batalha, id_batalha_atual) == 0) 
			{ 
				bool encontrou = false;
				// Verifica se o treinador já está na lista de campeões
				for (int k = 0; k < iterador_campeoes; k++)
				{
					if (strcmp(campeoes[k].id_treinador, r.id_treinador) == 0)
					{
						if (r.foi_maior_duracao)
							campeoes[k].vezes_mais_tempo++;

						if (r.foi_mais_derrotas)
							campeoes[k].vezes_mais_derrotados++;

						if (r.foi_mais_dano)
							campeoes[k].vezes_mais_dano++;
						
						encontrou = true;
						break;
					}
					// Encontrou uma posição vazia
				}
				if (!encontrou)
				{
					sprintf(campeoes[iterador_campeoes].id_treinador, "%s", r.id_treinador);
					
					if (r.foi_maior_duracao)
						campeoes[iterador_campeoes].vezes_mais_tempo = 1;
					else
						campeoes[iterador_campeoes].vezes_mais_tempo = 0;

					if (r.foi_mais_derrotas)
						campeoes[iterador_campeoes].vezes_mais_derrotados = 1;
					else
						campeoes[iterador_campeoes].vezes_mais_derrotados = 0;

					if (r.foi_mais_dano)
						campeoes[iterador_campeoes].vezes_mais_dano = 1;
					else
						campeoes[iterador_campeoes].vezes_mais_dano = 0;
					
						iterador_campeoes++;
				}	
			}
			
		}
		
	}
	if (iterador_campeoes == 0) 
	{
		printf(AVISO_NENHUM_REGISTRO_ENCONTRADO);
		return;
    }

	qsort(campeoes, iterador_campeoes, sizeof(Info_Treinador), qsort_info_treinador);	
	
	bool foi_removido = false;

	for (int i = 0; i < iterador_campeoes; i++)
	{
		int indice_campeao = busca_binaria(campeoes[i].id_treinador, treinadores_idx, qtd_registros_treinadores, sizeof(treinadores_index), qsort_treinadores_idx, false, 0);
		
		if (indice_campeao == -1 || treinadores_idx[indice_campeao].rrn == -1)
		{
			foi_removido = true;
			continue;
		}
		
		if (foi_removido)
		{
			printf(ERRO_TREINADOR_REMOVIDO, premio, campeoes[i].id_treinador, campeoes[i].vezes_mais_tempo, campeoes[i].vezes_mais_derrotados);
			continue;
		}
		
		printf(CONCEDER_PREMIO, campeoes[i].id_treinador, campeoes[i].vezes_mais_tempo, campeoes[i].vezes_mais_derrotados, premio);

		adicionar_bolsobolas(campeoes[i].id_treinador, premio, false);
		
		printf(ERRO_TREINADOR_REMOVIDO, premio, treinadores_idx[indice_campeao].id_treinador, campeoes[i].vezes_mais_tempo, campeoes[i].vezes_mais_derrotados);
	}
	
}

// ---------------- Busca pelo índice ----------------

void buscar_treinador_id_menu(char *id_treinador) {
	treinadores_index index;
	strcpy(index.id_treinador, id_treinador);
	int found = busca_binaria((void*)&index, treinadores_idx, qtd_registros_treinadores, sizeof(treinadores_index), qsort_treinadores_idx, true, 0);
	if(found == -1 || treinadores_idx[found].rrn < 0)
		printf(ERRO_REGISTRO_NAO_ENCONTRADO);
	else
		exibir_treinador(treinadores_idx[found].rrn);	
}

void buscar_bolsomon_id_menu(char *id_bolsomon) {
	int retorno_busca = busca_binaria(id_bolsomon, bolsomons_idx, qtd_registros_bolsomons, sizeof(bolsomons_index), qsort_bolsomons_idx, true, 0);
	if (retorno_busca == -1)
		printf(ERRO_REGISTRO_NAO_ENCONTRADO);
	else
		exibir_bolsomon(retorno_busca);
}

void buscar_batalha_id_menu(char *id_batalha) {
	int retorno_busca = busca_binaria(id_batalha, batalhas_idx, qtd_registros_batalhas, sizeof(batalhas_index), qsort_batalhas_idx, true, 0);
	if (retorno_busca == -1)
		printf(ERRO_REGISTRO_NAO_ENCONTRADO);
	else
		exibir_batalha(batalhas_idx[retorno_busca].rrn);
}

// ---------------- Listagem dos registros ----------------

void listar_treinadores_id_menu() {
	if(!qtd_registros_treinadores){
		printf(AVISO_NENHUM_REGISTRO_ENCONTRADO);
        return;
    }
	for(unsigned int i = 0; i < qtd_registros_treinadores; i++)
		exibir_treinador(treinadores_idx[i].rrn);
}

void listar_treinadores_bolsomons_menu(char *bolsomon){
	if(!qtd_registros_bolsomons){
		printf(AVISO_NENHUM_REGISTRO_ENCONTRADO);
        return;
    }
	for(unsigned int i = 0; i < qtd_registros_bolsomons; i++)
		exibir_bolsomon(bolsomons_idx[i].rrn);
}

void listar_bolsomons_compra_menu(char *id_treinador) {
    int retorno_treinador_idx = busca_binaria(id_treinador, treinadores_idx, qtd_registros_treinadores, sizeof(treinadores_index), qsort_treinadores_idx, false, 0);
    if (retorno_treinador_idx == -1 || treinadores_idx[retorno_treinador_idx].rrn < 0) {
        printf(ERRO_REGISTRO_NAO_ENCONTRADO);
        return;
    }

    Treinador t = recuperar_registro_treinador(treinadores_idx[retorno_treinador_idx].rrn);

    bool encontrou_um = false;
    int i = 0; 

    while (i < qtd_registros_bolsomons && preco_bolsomon_idx[i].preco_bolsobolas <= t.bolsobolas) {
        int bolsomon_idx_pos = busca_binaria(preco_bolsomon_idx[i].id_bolsomon, bolsomons_idx, qtd_registros_bolsomons, sizeof(bolsomons_index), qsort_bolsomons_idx, false, 0);
        
        if (bolsomon_idx_pos != -1) {
            exibir_bolsomon(bolsomons_idx[bolsomon_idx_pos].rrn);
            encontrou_um = true;
        }

        i++; 
    }

    if (!encontrou_um) {
        printf(AVISO_NENHUM_REGISTRO_ENCONTRADO);
    }
}

void listar_batalhas_periodo_menu(char *data_inicio, char *data_fim) {
	int retorno_inicio = busca_binaria(data_inicio, data_idx, qtd_registros_batalhas, sizeof(data_index), qsort_data_idx, true, 1);
	int retorno_fim = busca_binaria(data_fim, data_idx, qtd_registros_batalhas, sizeof(data_index), qsort_data_idx, true, -1);

	if (retorno_inicio == -1 || retorno_fim == -1) {
		printf(AVISO_NENHUM_REGISTRO_ENCONTRADO);
		return;
	}

	//Percorrer as datas dentro do intervalo e recuperar as batalhas
	for (int i = retorno_inicio; i <= retorno_fim; i++) {
		//Recuperar rrn da batalha
		int busca_batalha = busca_binaria(data_idx[i].id_batalha, batalhas_idx, qtd_registros_batalhas, sizeof(batalhas_index), qsort_batalhas_idx, false, 0);
		if (busca_batalha != -1) {
			exibir_batalha(batalhas_idx[busca_batalha].rrn);
		}
	}
}

// ---------------- Exibição dos arquivos de dados ----------------

void imprimir_arquivo_treinadores_menu() {
	if(!qtd_registros_treinadores){
		printf(ERRO_ARQUIVO_VAZIO);
        return;
    }
	printf("%s\n", ARQUIVO_TREINADORES);
}

void imprimir_arquivo_bolsomons_menu() {
	if(!qtd_registros_bolsomons){
		printf(ERRO_ARQUIVO_VAZIO);
        return;
    }
	printf("%s\n", ARQUIVO_BOLSOMONS);
}

void imprimir_arquivo_batalhas_menu() {
	if(!qtd_registros_batalhas){
		printf(ERRO_ARQUIVO_VAZIO);
		return;
	}
	printf("%s\n", ARQUIVO_BATALHAS);
} 

void imprimir_arquivo_resultados_menu() {
	if(!qtd_registros_resultados){
		printf(ERRO_ARQUIVO_VAZIO);
		return;
	}
	printf("%s\n", ARQUIVO_RESULTADOS);
}

void imprimir_arquivo_treinador_possui_bolsomon_menu() {
	if(!qtd_registros_treinador_possui_bolsomon){
		printf(ERRO_ARQUIVO_VAZIO);
		return;
	}
	printf("%s\n", ARQUIVO_TREINADOR_POSSUI_BOLSOMON);
}

// ---------------- Exibição dos índices ----------------

void imprimir_treinadores_idx_menu() {
	if(!treinadores_idx || !qtd_registros_treinadores){
		printf(ERRO_ARQUIVO_VAZIO);
        return;
    }
	for(unsigned i = 0; i < qtd_registros_treinadores; i++)
		printf("%s, %d\n", treinadores_idx[i].id_treinador, treinadores_idx[i].rrn);
}

void imprimir_bolsomons_idx_menu() {
	if(!bolsomons_idx || !qtd_registros_bolsomons){
		printf(ERRO_ARQUIVO_VAZIO);
        return;
    }
	for(unsigned i = 0; i < qtd_registros_bolsomons; i++)
		printf("%s, %d\n", bolsomons_idx[i].id_bolsomon, bolsomons_idx[i].rrn);
}

void imprimir_batalhas_idx_menu() {
	if(!batalhas_idx || !qtd_registros_batalhas){
		printf(ERRO_ARQUIVO_VAZIO);
		return;
	}
	for(unsigned i = 0; i < qtd_registros_batalhas; i++)
		printf("%s, %d\n", batalhas_idx[i].id_batalha, batalhas_idx[i].rrn);
}

void imprimir_resultados_idx_menu() {
	if(!resultados_idx || !qtd_registros_resultados){
		printf(ERRO_ARQUIVO_VAZIO);
		return;
	}
	for(unsigned i = 0; i < qtd_registros_resultados; i++)
		printf("%s, %s, %d\n", resultados_idx[i].id_treinador, resultados_idx[i].id_batalha,  resultados_idx[i].rrn);
}

void imprimir_treinador_possui_bolsomon_idx_menu() {
	if(!treinador_possui_bolsomon_idx || !qtd_registros_treinador_possui_bolsomon){
		printf(ERRO_ARQUIVO_VAZIO);
		return;
	}
	for(unsigned i = 0; i < qtd_registros_treinador_possui_bolsomon; i++)
		printf("%s, %s, %d\n", treinador_possui_bolsomon_idx[i].id_treinador, treinador_possui_bolsomon_idx[i].id_bolsomon, treinador_possui_bolsomon_idx[i].rrn);
}

void imprimir_preco_bolsomon_idx_menu() {
	if(!preco_bolsomon_idx || !qtd_registros_bolsomons){
		printf(ERRO_ARQUIVO_VAZIO);
        return;
    }
	for(unsigned i = 0; i < qtd_registros_bolsomons; i++)
		printf("%.2lf, %s\n", preco_bolsomon_idx[i].preco_bolsobolas, preco_bolsomon_idx[i].id_bolsomon);
}

void imprimir_data_idx_menu() {
	if(!data_idx || !qtd_registros_batalhas){
		printf(ERRO_ARQUIVO_VAZIO);
		return;
	}
	for(unsigned i = 0; i < qtd_registros_batalhas; i++)
		printf("%s, %s\n", data_idx[i].id_batalha, data_idx[i].inicio);
}

void imprimir_treinador_bolsomons_secundario_idx_menu() {
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "imprimir_treinador_bolsomons_secundario_idx_menu()");
}

void imprimir_treinador_bolsomons_primario_idx_menu() {
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "imprimir_treinador_bolsomons_primario_idx_menu()");
}

// ---------------- Liberação de espaço e memória ----------------

void liberar_espaco_menu() {
	//Reorganizar o índice
	if (qtd_registros_treinadores == 0) 
	{
		printf(SUCESSO);
		return;
	}
	int qtd_treinadores_antiga = qtd_registros_treinadores;
	
	//Refazendo o índice sem os registros removidos
	for (int i = 0; i < qtd_treinadores_antiga; i++) 
	{
		if (treinadores_idx[i].rrn == -1) 
		{
			strcpy(treinadores_idx[i].id_treinador, treinadores_idx[qtd_registros_treinadores - 1].id_treinador);
			treinadores_idx[i].rrn = treinadores_idx[qtd_registros_treinadores - 1].rrn;
			qtd_registros_treinadores--;
		}
	}
	
	qsort(treinadores_idx, qtd_registros_treinadores, sizeof(treinadores_index), qsort_treinadores_idx);
	
	//Reorganizando o arquivo de dados
	char novo_arquivo[TAM_ARQUIVO_TREINADORES];
	novo_arquivo[0] = '\0';
	
	int novo_indice = 0;
	for (int i = 0; i < qtd_treinadores_antiga; i++)
	{
		if (strncmp(ARQUIVO_TREINADORES + (i * TAM_REGISTRO_TREINADOR), "*|", 2) != 0) {
			strncat(novo_arquivo, ARQUIVO_TREINADORES + i * TAM_REGISTRO_TREINADOR, TAM_REGISTRO_TREINADOR);

			//Atualizando o rrn no índice
			char id_treinador[TAM_ID_TREINADOR];
			strncpy(id_treinador, ARQUIVO_TREINADORES + (i * TAM_REGISTRO_TREINADOR), TAM_ID_TREINADOR - 1);
			id_treinador[TAM_ID_TREINADOR - 1] = '\0';
			int resultado_busca = busca_binaria(id_treinador, treinadores_idx, qtd_registros_treinadores, sizeof(treinadores_index), qsort_treinadores_idx, false, 0);
			treinadores_idx[resultado_busca].rrn = novo_indice;
			novo_indice++;
		}

	}

	sprintf(ARQUIVO_TREINADORES, "%s", novo_arquivo);
	printf(SUCESSO);
}

void liberar_memoria_menu() {
	free(treinadores_idx);
	free(bolsomons_idx);
	free(batalhas_idx);
	free(resultados_idx);
	free(data_idx);
	free(preco_bolsomon_idx);
}

// ---------------- Manipulação da lista invertida ----------------

void inverted_list_insert(char *chave_secundaria, char *chave_primaria, inverted_list *t) {
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "inverted_list_insert()");
}

bool inverted_list_secondary_search(int *result, bool exibir_caminho, char *chave_secundaria, inverted_list *t) {
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "inverted_list_secondary_search()");
	return false;
}

int inverted_list_primary_search(char result[][TAM_CHAVE_TREINADOR_BOLSOMON_PRIMARIO_IDX], bool exibir_caminho, int indice, int *indice_final, inverted_list *t) {
	/*IMPLEMENTE A FUNÇÃO AQUI*/
	printf(ERRO_NAO_IMPLEMENTADO, "inverted_list_primary_search()");
	return 0;
}

// ---------------- Busca binária ----------------

int busca_binaria(const void *key, const void *base0, size_t nmemb, size_t size, int (*compar)(const void *, const void *), bool exibir_caminho, int retorno_se_nao_encontrado) {
	// Converte o ponteiro void* para um ponteiro de char* para permitir aritmética de ponteiros.
    int inicio = 0;
    int fim = nmemb - 1;


    if (exibir_caminho) 
        printf("Registros percorridos:");

    while (fim >= inicio) {
        // Calcula o meio para evitar overflow com (inicio + fim) / 2
		// +1 para arredondar para cima em caso de números pares
        int meio = (inicio + fim + 1) / 2;

        if (exibir_caminho) 
            printf(" %d", meio);

        // Obtém um ponteiro para o elemento do meio do array.

        int resultado_comparacao = compar(key, base0 + (meio * size));

		// A chave está na metade superior
        if (resultado_comparacao > 0) {
			inicio = meio + 1;

		}
        
        // A chave está na metade inferior
		else if (resultado_comparacao < 0) {
            fim = meio - 1;

		}
        
        // Elemento encontrado  
		else
		{
            if (exibir_caminho)
                printf("\n");

			return meio;
		}
    }

    if (exibir_caminho) 
	printf("\n");
    
	
    // Elemento não encontrado
    if (retorno_se_nao_encontrado == 0) 
        return -1;

    // Se retorno_se_nao_encontrado == -1, vai retorna predecessor
    else if (retorno_se_nao_encontrado == -1) 
	{
        if (fim >= 0) 
            return fim;
        else 
            return -1;
    }

    // Se retorno_se_nao_encontrado == 1, vai retorna sucessor
    else if (retorno_se_nao_encontrado == 1) 
	{
        if (inicio < nmemb) 
            return inicio;
        else 
            return -1;
        
    }
    
    // Para qualquer outro valor de treinado
    return -1;
}

// ---------------- Funções SET de arquivos ----------------

void set_arquivo_treinadores(char* novos_dados, size_t tamanho){
    memcpy(ARQUIVO_TREINADORES, novos_dados, tamanho);
    qtd_registros_treinadores = tamanho / TAM_REGISTRO_TREINADOR;
}

void set_arquivo_bolsomons(char* novos_dados, size_t tamanho){
    memcpy(ARQUIVO_BOLSOMONS, novos_dados, tamanho);
    qtd_registros_bolsomons = tamanho / TAM_REGISTRO_BOLSOMON;
}

void set_arquivo_batalhas(char* novos_dados, size_t tamanho){
    memcpy(ARQUIVO_BATALHAS, novos_dados, tamanho);
    qtd_registros_batalhas = tamanho / TAM_REGISTRO_BATALHA;
}

void set_arquivo_resultados(char* novos_dados, size_t tamanho){
    memcpy(ARQUIVO_RESULTADOS, novos_dados, tamanho);
    qtd_registros_resultados = tamanho / TAM_REGISTRO_RESULTADO;
}

void set_arquivo_treinador_possui_bolsomon(char* novos_dados, size_t tamanho){
    memcpy(ARQUIVO_TREINADOR_POSSUI_BOLSOMON, novos_dados, tamanho);
    qtd_registros_treinador_possui_bolsomon = tamanho / TAM_REGISTRO_TREINADOR_POSSUI_BOLSOMON;
}

// ---------------- Implementação das suas próprias funções auxiliares ----------------

int qsort_int(const void *a, const void *b) {
	if (*(int*)a > *(int*)b)
		return 1;
	
	if (*(int*)a < *(int*)b)
		return -1;
	
	return 0;
}

int encontrar_maior_duracao(Dados_Treinador dados_treinadores[], int n)
{
	int maior = 0;
	for (int i = 1; i < n; i++)
	{
		if (dados_treinadores[i].duracao > dados_treinadores[maior].duracao)
			maior = i;
	}
	return maior;
}

int encontrar_maior_eliminacoes(Dados_Treinador dados_treinadores[], int n)
{
	int maior = 0;
	for (int i = 1; i < n; i++)
	{
		if (dados_treinadores[i].eliminacoes > dados_treinadores[maior].eliminacoes)
			maior = i;
	}
	return maior;
}

int encontrar_maior_dano(Dados_Treinador dados_treinadores[], int n)
{
	int maior = 0;
	for (int i = 1; i < n; i++)
	{
		if (dados_treinadores[i].dano > dados_treinadores[maior].dano)
			maior = i;
	}
	return maior;
}

