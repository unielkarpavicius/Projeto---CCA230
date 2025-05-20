#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <stdbool.h>
#define MAX_PRIORITARIO 20 // Define o tamanho máximo da fila prioritária
#define LEN 100 // Define o tamanho máximo para strings
#ifdef _WIN32
#define strcasecmp _stricmp
#endif

typedef struct Data { // Estrutura para armazenar informações de data
    int dia;
    int mes;
    int ano;
} Data;

typedef struct Celula { // Estrutura principal do paciente
    char nome[LEN]; // Nome do paciente
    char RG[LEN]; // RG do paciente
    int idade; // Idade do paciente
    Data *entrada; // Data de entrada do paciente
    struct Celula *proxima; // Para lista encadeada de todos os pacientes
} Celula;

typedef struct Operacao { // Estrutura para armazenar operações (para desfazer)
    char tipo[10];// Vai armazenar o tipo de ação feita (ENFILEIRAR/REMOVER)
    Celula *paciente;// Pega o paciente envolvido com a ação
    struct Operacao *prox;// Proxima operação na pilha
} Operacao;

typedef struct { // Estrutura para a pilha de operações
    Operacao *topo; // Ponteiro para o top da pilha
} Pilha;

typedef struct FilaP { // Estrutura para a fila prioritária
    Celula* heap[MAX_PRIORITARIO]; // Heap para armazenar pacientes prioritários
    int qtde; // Quantidade de pacientes na fila
}FilaP;

typedef struct Lista { // Estrutura para a lista de pacientes
    int qtde; // Quantidade de pacientes na lista
    Celula *primeiro; // Ponteiro para o primeiro paciente da lista
} Lista;

typedef struct Efila{ // Estrutura para a fila 
    Celula *Dados; // Dados do paciente
    struct Efila *Proximo; // Ponteiro para o próximo elemento da fila
}Efila;

typedef struct Fila { // Fila normal de atendimento 
    Efila *Head; // Ponteiro para o início da fila
    Efila *Tail; // Ponteiro para o fim da fila
    int Qtde; 
}Fila;

typedef struct NO{ // Estrutura para o nó da arvore binária
    Celula* paciente;
    struct NO *esq, *dir; // Ponteiros para os filhos esquerdo e direito
}NO;

typedef struct { // Estrutura da arvore binária de busca
    NO *raiz; // Ponteiro para a raiz da árvore
    int qtde;
}Arvore;

Fila *inicializa_fila(){ // Função para a inicialização da fila 
    Fila *fila = malloc(sizeof(Fila)); // Aloca memória para a fila
    if(fila == NULL){
        printf("Erro de alocacao de memoria!\n");
        exit(1); // Encerra o programa em caso de erro
    }
    fila->Qtde = 0;
    fila->Head = NULL;
    fila->Tail = NULL;
    return fila;
}

Lista *inicializa_lista() { // Funções de inicialização a lista de pacientes
    Lista *lista = malloc(sizeof(Lista)); // Aloca memória para a lista
    if (lista == NULL) {
        printf("Erro de alocacao de memoria!\n");
        exit(1);
    }
    lista->primeiro = NULL;
    lista->qtde = 0;
    return lista;
}

Celula *inicializa_celula() { // Inicializa a celula de paciente
    Celula *novo = malloc(sizeof(Celula)); // Aloca memória para a célula
    if (novo == NULL) {
        printf("Erro de alocacao de memoria!\n");
        exit(1);
    }
    novo->proxima = NULL;
    strcpy(novo->nome, "-"); // Inicializa o nome com "-"
    strcpy(novo->RG, "000000000-00"); // Inicializa o RG com "000000000-00"
    novo->idade = 0;
    novo->entrada = malloc(sizeof(Data)); // Aloca memória para a data de entrada
    if (novo->entrada == NULL) {
        printf("Erro de alocação de memoria!\n");
        exit(1);
    }
    novo->entrada->dia = 0;
    novo->entrada->mes = 0;
    novo->entrada->ano = 0;
    return novo;
}

Efila *inicializa_efila(){ // Função para inicializar um elemento da fila normal
    Efila *nova = (Efila*)malloc(sizeof(Efila));
    if (nova == NULL) {
        printf("Erro de alocacao de memoria!\n");
        exit(1);
    }
    nova->Dados = inicializa_celula(); // Inicializa a célula de dados
    nova->Proximo = NULL; 
    return nova;
}

Pilha* inicializa_pilha() { //Função para a inicialização da pilha de operações
    Pilha* p = malloc(sizeof(Pilha));
    p->topo = NULL;
    return p;
}

void limpar_espacos(char *str) { // Função para limpar espaços extras no nome
    int len = strlen(str);
    while (len > 0 && (str[len-1] == ' ' || str[len-1] == '\n')){
        str[--len] = '\0';
    }
}

Celula *buscar_paciente(Lista *l, const char *nome) { // Função para buscar paciente por nome
    Celula *atual = l->primeiro;
    while (atual != NULL) {
        if (strcmp(atual->nome, nome) == 0) {
            return atual; // Retorna o paciente se encontrado
        }
        atual = atual->proxima;
    }
    return NULL;
}

void cadastrar_paciente(Lista *l, char *nome, char *rg, int idade, Data *entrada) { // Função para cadastrar um paciente
    Celula *novo = inicializa_celula(); // Cria uma nova célula para o paciente
    strcpy(novo->nome, nome);
    strcpy(novo->RG, rg);
    novo->idade = idade;
    novo->entrada->dia = entrada->dia;
    novo->entrada->mes = entrada->mes;
    novo->entrada->ano = entrada->ano;
    
    novo->proxima = l->primeiro; // Insere o novo paciente no início da lista
    l->primeiro = novo;
    l->qtde++;
}

void salvar_pacientes(Lista *l) { // Função para escrever os pacientes no arquivo
    FILE *arquivo = fopen("pacientes.txt", "w"); // Abre o arquivo para escrita
    if (arquivo == NULL) {
        printf("Erro ao abrir arquivo para escrita!\n");
        return;
    }
    Celula *atual = l->primeiro;
    while (atual != NULL) {
        fprintf(arquivo, "%s\n", atual->nome);
        fprintf(arquivo, "%s\n", atual->RG);
        fprintf(arquivo, "%d\n", atual->idade);
        fprintf(arquivo, "%d %d %d\n", atual->entrada->dia, atual->entrada->mes, atual->entrada->ano);
        atual = atual->proxima;
    }
    fclose(arquivo);
}

void salvar_pacientes_remov(Lista *l) { // Função para atualizar os pacientes no arquivo
    FILE *arquivo = fopen("pacientes.txt", "w"); // Abre o arquivo para escrita
    if (arquivo == NULL) {
        printf("Erro ao abrir arquivo para escrita!\n");
        return;
    }
    Celula *atual = l->primeiro;
    while (atual != NULL) {
        fprintf(arquivo, "%s\n", atual->nome);
        fprintf(arquivo, "%s\n", atual->RG);
        fprintf(arquivo, "%d\n", atual->idade);
        fprintf(arquivo, "%d %d %d\n", atual->entrada->dia, atual->entrada->mes, atual->entrada->ano);
        atual = atual->proxima;
    }
    fclose(arquivo);
}

void remover_paciente(Lista *l, char *nome) { // Função para remover paciente
    Celula *atual = l->primeiro;
    Celula *anterior = NULL;
    limpar_espacos(nome);
    while (atual != NULL && strcmp(atual->nome, nome) != 0) {
        anterior = atual;
        atual = atual->proxima;
    }
    if (atual == NULL) {
        printf("Nao foi encontrado nenhum paciente com este nome!\n");
        return;
    }
    if (anterior == NULL) {
        l->primeiro = atual->proxima; // Remove o primeiro paciente
    } else {
        anterior->proxima = atual->proxima; // Remove um paciente do meio ou fim
    }
    free(atual->entrada); // Libera a memória da data de entrada
    free(atual); // Libera a memória do paciente
    l->qtde--;
    salvar_pacientes_remov(l); // Atualiza o arquivo
    printf("Paciente removido com sucesso!\n");
}

void atualiza_dados(Lista *l) { // Função para atualizar dados
    char buffer[256], nome[LEN];
    char novo_nome[LEN], novo_rg[LEN];
    int nova_idade;
    Data nova_entrada;
    char opcao;

    printf("Digite o nome do paciente que deseja atualizar: ");
    fgets(nome, LEN, stdin);
    limpar_espacos(nome);
    Celula *paciente = buscar_paciente(l, nome);
    if (paciente == NULL) {
        printf("Paciente nao encontrado!\n");
        return;
    }
    while (1) {
        printf("\n==========================================\n");
        printf("           MENU DE ATUALIZAÇÃO\n");
        printf("\n==========================================\n");
        printf("1 - Atualizar Nome\n");
        printf("2 - Atualizar RG\n");
        printf("3 - Atualizar Idade\n");
        printf("4 - Atualizar Data de Entrada\n");
        printf("0 - Voltar ao Menu Principal\n");
        printf("\n==========================================\n");
        printf("Escolha: ");
        fgets(buffer, sizeof(buffer), stdin);
        opcao = buffer[0];
        switch (opcao) {
            case '1':
                printf("Digite o novo nome: ");
                fgets(novo_nome, LEN, stdin);
                limpar_espacos(novo_nome);
                strcpy(paciente->nome, novo_nome);
                salvar_pacientes_remov(l);
                printf("Nome atualizado com sucesso!\n");
                break;
            case '2':
                printf("Digite o novo RG: ");
                fgets(novo_rg, LEN, stdin);
                limpar_espacos(novo_rg);
                strcpy(paciente->RG, novo_rg);
                salvar_pacientes_remov(l);
                printf("RG atualizado com sucesso!\n");
                break;
            case '3':
                printf("Digite a nova idade: ");
                scanf("%d", &nova_idade);
                paciente->idade = nova_idade;
                getchar(); // limpa '\n'
                salvar_pacientes_remov(l);
                printf("Idade atualizada com sucesso!\n");
                break;
            case '4':
                printf("Digite o novo dia de entrada: ");
                scanf("%d", &nova_entrada.dia);
                printf("Digite o novo mes de entrada: ");
                scanf("%d", &nova_entrada.mes);
                printf("Digite o novo ano de entrada: ");
                scanf("%d", &nova_entrada.ano);
                getchar(); // limpa '\n'
                paciente->entrada->dia = nova_entrada.dia;
                paciente->entrada->mes = nova_entrada.mes;
                paciente->entrada->ano = nova_entrada.ano;
                salvar_pacientes_remov(l);
                printf("Data de entrada atualizada com sucesso!\n");
                break;
            case '0':
                return;
            default:
                printf("Opção invalida!\n");
        }
    }
}

void ler_pacientes(Lista *l) { // Função para ler os dados dos pacientes do arquivo
    FILE *arquivo = fopen("pacientes.txt", "r"); // Abre o arquivo para leitura
    if (arquivo == NULL) {
        printf("Erro ao abrir arquivo para leitura!\n");
        return;
    }
    char nome[LEN], rg[LEN];
    int idade;
    Data entrada;
    while (fscanf(arquivo, "%99[^\n]\n", nome) == 1) { // Ler dados dos pacientes
        fscanf(arquivo, "%99[^\n]\n", rg);
        fscanf(arquivo, "%d\n", &idade);
        fscanf(arquivo, "%d %d %d\n", &entrada.dia, &entrada.mes, &entrada.ano);
        cadastrar_paciente(l, nome, rg, idade, &entrada); // Cadastrar paciente lido
    }
    fclose(arquivo);
}

void mostrar_dados(Lista *l) { // Função para mostrar todos os pacientes
    if (l->qtde == 0) {
        printf("Nao existem pacientes cadastrados!\n");
        return;
    }
    Celula *atual = l->primeiro;
    while (atual != NULL) {
        printf("\n==========================================\n");
        printf("NOME: %s\n", atual->nome);
        printf("RG: %s\n", atual->RG);
        printf("IDADE: %d anos\n", atual->idade);
        printf("DATA DE ENTRADA: %02d/%02d/%04d\n", atual->entrada->dia, atual->entrada->mes, atual->entrada->ano);
        printf("\n==========================================\n");
        atual = atual->proxima;
    }
    printf("\n");
}

Celula* retornar_paciente_arquivo(const char* nome_busca) { // Função para retornar um paciente do arquivo pelo nome
    FILE *arquivo = fopen("pacientes.txt", "r"); // Abre o arquivo para leitura
    if (arquivo == NULL) { // Verifica se o arquivo existe 
        printf("Erro ao abrir arquivo para leitura!\n");
        return NULL;
    }
    char nome[LEN], rg[LEN];
    int idade;
    Data entrada;
    Celula* paciente_encontrado = NULL;
    // Percorre o arquivo linha por linha
    while (fscanf(arquivo, "%99[^\n]\n", nome) == 1) { // Le 99 caracteres do arquivo pulando a quebra de linha e armazenando no "nome"
        fscanf(arquivo, "%99[^\n]\n", rg);
        fscanf(arquivo, "%d\n", &idade);
        fscanf(arquivo, "%d %d %d\n", &entrada.dia, &entrada.mes, &entrada.ano);
        if (strcasecmp(nome, nome_busca) == 0) { // Compara com o nome buscado
            paciente_encontrado = inicializa_celula(); // Inicializa uma nova celula para armazenar os dados do paciente
            strcpy(paciente_encontrado->nome, nome); // Armazena o nome encontrado dentro do nome da celula "paciente_encontrado"
            strcpy(paciente_encontrado->RG, rg); // Armazena o Rg encontrado dentro do Rg da celula "paciente_encontrado"
            paciente_encontrado->idade = idade; // Armazena a idade encontrada dentro da idade da celula "paciente_encontrado"
            paciente_encontrado->entrada->dia = entrada.dia; // Faz a mesma coisa para os valores da data de entrada no hospital 
            paciente_encontrado->entrada->mes = entrada.mes;
            paciente_encontrado->entrada->ano = entrada.ano;
            break;
        }
    }
    fclose(arquivo);
    if (paciente_encontrado == NULL) {
        printf("Paciente nao encontrado no arquivo!\n");
    } 
    return paciente_encontrado;
}

void consulta_paciente(Lista *L) { // Função para consultar os dados de um paciente pelo nome
    char nome[LEN];
    printf("Digite o nome do paciente que deseja consultar: ");
    fgets(nome, LEN, stdin); // Pega a respota do usuário
    limpar_espacos(nome); // Limpa espaço vazia que pode ser deixado pelo o usuário
    Celula *atual = buscar_paciente(L, nome);
    if (atual == NULL) {
        printf("Paciente nao encontrado!\n");
        return;
    }
    if (atual != NULL) {
        printf("\n==========================================\n");
        printf("NOME: %s\n", atual->nome);
        printf("RG: %s\n", atual->RG);
        printf("IDADE: %d anos\n", atual->idade);
        printf("DATA DE ENTRADA: %02d/%02d/%04d\n", atual->entrada->dia, atual->entrada->mes, atual->entrada->ano);
        printf("\n==========================================\n");
    }
    printf("\n");
}

void liberar_lista(Lista *l) { // Função para liberar toda a lista
    Celula *atual = l->primeiro;
    while (atual != NULL) {
        Celula *temp = atual;
        atual = atual->proxima;
        free(temp->entrada); // Libera a memória da data de entrada
        free(temp); // Libera a memória da célula do paciente
    }
    free(l); // Libera a memória da lista
}

int menu_cadastro(Lista *lista) { // Função para o menu de cadastro de pacientes
    char ch, buffer[256];
    char nome[LEN], rg[LEN];
    int idade;
    Data entrada;
    while (1) {
        printf("\n==========================================\n");
        printf("             MENU DO CADASTRO");
        printf("\n==========================================\n");
        printf("1 - Cadastrar Paciente\n");
        printf("2 - Mostrar Pacientes\n");
        printf("3 - Remover Paciente\n");
        printf("4 - Atualizar Dados\n");
        printf("5 - Consultar Paciente\n");
        printf("0 - Voltar Para O Menu Do Sistema\n");
        printf("\n==========================================\n");
        printf("Escolha: ");
        fgets(buffer, sizeof(buffer), stdin);
        ch = buffer[0];
        switch (ch) {
            case '1':
                printf("Digite o nome do paciente: ");
                fgets(nome, LEN, stdin);
                limpar_espacos(nome);
                printf("Digite o RG do paciente: ");
                fgets(rg, LEN, stdin);
                limpar_espacos(rg);
                printf("Digite a idade do paciente: ");
                scanf("%d", &idade);
                printf("Digite o dia da entrada: ");
                scanf("%d", &entrada.dia);
                printf("Digite o mes da entrada: ");
                scanf("%d", &entrada.mes);
                printf("Digite o ano da entrada: ");
                scanf("%d", &entrada.ano);
                getchar(); // limpa '\n'
                cadastrar_paciente(lista, nome, rg, idade, &entrada);
                salvar_pacientes(lista);
                printf("Paciente cadastrado com sucesso!\n");
                break;
            case '2':
                mostrar_dados(lista);
                break;
            case '3':
                printf("Digite o nome completo do paciente a ser removido: ");
                fgets(nome, LEN, stdin);
                limpar_espacos(nome);
                remover_paciente(lista, nome);
                break;
            case '4':
                atualiza_dados(lista);
                break;
            case '5':
                consulta_paciente(lista);
                break;
            case '0':
                liberar_lista(lista);
                return 0;
            default:
                printf("Opção invalida!\n");
        }
    }
}

void enfilerar(Fila* fila, const char nome[], Pilha*pilha) { /// Função para enfileirar um paciente na fila normal
    Celula* paciente = retornar_paciente_arquivo(nome); // Pega os dados do paciente salvo no arquivo "paciente.txt" através da fução "retorna_paciente_arquivo"
    if (paciente == NULL) {
        return;
    }
    Efila *novo = inicializa_efila(); // Cria uma nova celula para a fila
    // Copia os dados do paciente para a nova célula da fila
    strcpy(novo->Dados->nome, paciente->nome); // Passa o nome do paciente para a celula "novo"
    strcpy(novo->Dados->RG, paciente->RG); // Passa o Rg do paciente para a celula "novo"
    novo->Dados->idade = paciente->idade; // Passa a idade do paciente para a celula "novo"
    novo->Dados->entrada->dia = paciente->entrada->dia; // Passa o dia de entrada do paciente no hospital para a celula "novo"
    novo->Dados->entrada->mes = paciente->entrada->mes; // Passa o mes de entrada do paciente no hospital para a celula "novo"
    novo->Dados->entrada->ano = paciente->entrada->ano; // Passa o ano de entrada do paciente no hospital para a celula "novo"
    Celula* copia = inicializa_celula(); // Cria uma copia segura do paciente para que nao exista conflito com o "free(paciente)"
    strcpy(copia->nome, paciente->nome);
    strcpy(copia->RG, paciente->RG);
    copia->idade = paciente->idade;
    copia->entrada->dia = paciente->entrada->dia;
    copia->entrada->mes = paciente->entrada->mes;
    copia->entrada->ano = paciente->entrada->ano;
    push_operacao(pilha, "ENFILEIRAR", copia); // registra o ENFILEIRAR na pilha 
    // Libera a memória do paciente retornado pela consulta
    free(paciente->entrada);
    free(paciente);
    if(fila->Qtde == 0) {
        fila->Head = novo; 
    } else { 
        fila->Tail->Proximo = novo; 
    }
    fila->Tail = novo; 
    fila->Qtde++; 
    printf("O paciente foi colocado na fila com sucesso!\n");
}

void remover_da_fila(Fila *fila,Pilha *pilha){ // Função para remover um paciente da fila normal
    if (fila->Qtde == 0){ // Verifica se a fila esta vazia
        printf("Fila Vazia\n");
        return;
    }
    Efila *temp = fila->Head; // Armazena o primeiro elemento da fila
    Celula* backup = inicializa_celula(); // Cria uma cópia do paciente ANTES de liberar a memória
    strcpy(backup->nome, temp->Dados->nome);
    strcpy(backup->RG, temp->Dados->RG);
    backup->idade = temp->Dados->idade;
    backup->entrada->dia = temp->Dados->entrada->dia;
    backup->entrada->mes = temp->Dados->entrada->mes;
    backup->entrada->ano = temp->Dados->entrada->ano;
    push_operacao(pilha, "REMOVER", backup);//Armazena a operação na pilha para poder desfazer depois
    fila->Head = fila->Head->Proximo; // Faz com que o segundo da fila se torne o primeiro e o antigo primero seja excluido da fila
    if(fila->Head == NULL){ // Caso a fila fique vazia após a remoção
        fila->Tail = NULL;
    }
    free(temp->Dados->entrada); // Libera a entrada do hospital da memoria 
    free(temp->Dados); // Libera os dados do paciente da memoria
    free(temp); // Libera o espaço da memoria do computador
    fila->Qtde--;
    printf("Paciente removido da fila com sucesso!");
}

void Mostra_Fila(Fila *fila){ //Mostra os pacientes da fila 
    Efila *atual = fila->Head; // Cria o incio da fila 
    printf("Proximo paciente: ");
    while(atual!=NULL){ // Percore toda a lista
        printf("\n==========================================\n");
        printf("NOME: %s\n", atual->Dados->nome); // Imprime o "nome" da variavel "atual"
        printf("RG: %s\n", atual->Dados->RG); // Imprime o "RG" da variavel "atual"
        printf("IDADE: %d anos\n", atual->Dados->idade); // Imprime a "idade" da variavel "atual"
        printf("DATA DE ENTRADA: %02d/%02d/%04d\n", atual->Dados->entrada->dia, atual->Dados->entrada->mes, atual->Dados->entrada->ano); // Imprime a "Entrada" da variavel "atual"
        printf("\n==========================================\n");
        atual = atual->Proximo; // Vai para o proximo elemento da lista
    }
    printf("Final da Fila");
    printf("\n");
    }

void liberar_fila(Fila*fila){ // Função para liberar a memoria da Fila, utilizada após a finaliza.
    Efila *atual = fila->Head; 
    while(atual!=NULL){ 
    Efila *temp = atual; // Celula temporaria para a liberação de memoria da celula atual. 
    atual = atual->Proximo; // avança pela fila ate o final do WHILE.
    free(temp->Dados->entrada); // Libera os valores de entrada (dia, mes e ano). 
    free(temp->Dados); // Libera os dados (nome, idade etc..)
    free(temp); // Libera o espaço que a celula ocupa na memoria.
    }
    free(fila); // Retorna a lista vazia logo depois do termino das atividades do menu.   
} 

void trocar_dados(Celula **a, Celula **b) { // Função para trocar os dados de dois pacientes (usada na fila prioritária)
    Celula *temp = *a;
    *a = *b;
    *b = temp;
}

// Funções para calcular os índices dos filhos e pai em um heap
int filho_esquerda(int pai) {
    return (2 * pai + 1);
}   
int filho_direita(int pai) {
    return (2 * pai + 2);
}
int indice_pai(int i){
    return (i-1)/2;
}
int ultimo_pai(FilaP *h) {
    return ((h->qtde / 2) - 1);
}

void peneirar_prioridade(FilaP *f, int pai) { // Função para "peneirar" um elemento para a posição correta no heap (fila prioritária)
    int fe = filho_esquerda(pai);
    int fd = filho_direita(pai);
    int maior = pai; 
    if (fe < f->qtde && f->heap[fe]->idade > f->heap[maior]->idade) { // Verifica se filho esquerdo existe e é mais velho que o pai
        maior = fe;
    }
    if (fd < f->qtde&& f->heap[fd]->idade > f->heap[maior]->idade) { // Verifica se filho direito existe e é mais velho que o atual maior
        maior = fd;
    }   
    if (maior != pai) { // Se algum filho é mais velho ele faz a troca e continua
        trocar_dados(&f->heap[pai], &f->heap[maior]);
        peneirar_prioridade(f, maior);
    }
}
  
void construir_heap_prioridade(FilaP* f) { // Organiza a fila apartir do ultimo pai
    for (int i = ultimo_pai(f); i >= 0; i--) {
        peneirar_prioridade(f, i);
    }
}

void adicionar_na_heap(FilaP* f) { // Função para adicionar um paciente na fila prioritária
    if (f->qtde >= MAX_PRIORITARIO) {
        printf("A fila prioritaria já está cheia (máximo de %d pacientes).\n", MAX_PRIORITARIO);
        return;
    }
    char nome[LEN];
    printf("Digite o nome do paciente para adicionar a fila prioritaria: ");
    fgets(nome, LEN, stdin);
    limpar_espacos(nome); // Tira o /n do console 
    Celula* paciente = retornar_paciente_arquivo(nome);
    if (!paciente) {
        printf("Paciente nao encontrado no arquivo.\n");
        return;
    }
    f->heap[f->qtde] = paciente; // Insere o paciente no final do vetor heap
    f->qtde++;
    construir_heap_prioridade(f); // Reconstrói o heap por completo
    printf("Paciente %s (Idade: %d) foi adicionado à fila prioritaria.\n", paciente->nome, paciente->idade);
}

void remover_maior_prioridade(FilaP* f) { // Função para remover o paciente com maior prioridade (mais velho) da fila prioritária
    if (f->qtde == 0) {
        printf("Fila prioritária vazia!\n");
        return;
    }
    Celula* paciente = f->heap[0]; // O paciente com maior prioridade é a raiz do heap
    printf("Atendendo paciente prioritario: %s (Idade: %d)\n", paciente->nome, paciente->idade);
    f->heap[0] = f->heap[f->qtde - 1]; // Substitui a raiz pelo último elemento
    f->qtde--;
    peneirar_prioridade(f, 0); // Reorganiza o heap
    free(paciente->entrada); // Libera a memória do paciente
    free(paciente);
}
    
void mostrar_heap(FilaP* f) { // Função para mostrar os pacientes na fila prioritária
    if (f->qtde == 0) {
        printf("Fila prioritaria vazia!\n");
        return;
    }
    printf("\n=== Fila Prioritaria (criterio por idade) ===\n");
    for (int i = 0; i < f->qtde; i++) {
        Celula* c = f->heap[i];
        printf("Nome: %s | Idade: %d | RG: %s | Entrada: %02d/%02d/%04d\n",c->nome, c->idade, c->RG,c->entrada->dia, c->entrada->mes, c->entrada->ano);
    }
}

void Sobre(){ // Função para exibir informações sobre 
    printf("\n==========================================\n");
    printf("             DADOS DOS ALUNOS");
    printf("\n==========================================\n");
        printf("Nome: Luana Nascimento Karpavicius\n");
    printf("Nome: Matheus Marcondes Pereira Rufino\n");
    printf("Ciclo: 7\n");
    printf("Curso: Engenharia de Robos\n");
    printf("Disciplina: Estrutura de dados\n");
    printf("Data: 08/05/2025");
    printf("\n==========================================\n");
}

typedef int (*ComparadorPaciente)(Celula*, Celula*); // Define o tipo de função para comparar pacientes
NO* inserir_na_arvore(NO* raiz, Celula* paciente, ComparadorPaciente cmp) { // Função para inserir um paciente na árvore binária de busca
    if (raiz == NULL) { // Se a árvore estiver vazia, cria um novo nó
        NO* novo = (NO*)malloc(sizeof(NO));
        novo->paciente = paciente;
        novo->esq = novo->dir = NULL;
        return novo;
    }
    if (cmp(paciente, raiz->paciente) < 0){ // Se o paciente for "menor", insere na esquerda
        raiz->esq = inserir_na_arvore(raiz->esq, paciente, cmp);
    } else { // Caso contrário, insere na direita
        raiz->dir = inserir_na_arvore(raiz->dir, paciente, cmp);
    }
    return raiz;
}

void in_order(NO* raiz) { // Função para percorrer a árvore em ordem (in-order) e imprimir os pacientes
    if (raiz != NULL) {
        in_order(raiz->esq);
        printf("%s - Idade: %d - Entrada: %02d/%02d/%04d\n", raiz->paciente->nome, raiz->paciente->idade, raiz->paciente->entrada->dia, raiz->paciente->entrada->mes, raiz->paciente->entrada->ano);
        in_order(raiz->dir);
    }
}

void inserir_pacientes_em_arvore(Arvore* arvore, ComparadorPaciente cmp) { // Função para inserir pacientes do arquivo na árvore binária de busca
    FILE* arq = fopen("pacientes.txt", "r");
    if (!arq) {
        printf("Erro ao abrir pacientes.txt\n");
        return;
    }
    char nome[LEN], rg[LEN];
    int idade;
    Data entrada;
    while (fscanf(arq, " %99[^\n]\n", nome) == 1) {
        fscanf(arq, " %99[^\n]\n", rg);
        fscanf(arq, "%d\n", &idade);
        fscanf(arq, "%d %d %d\n", &entrada.dia, &entrada.mes, &entrada.ano);
        Celula* novo = inicializa_celula();
        strcpy(novo->nome, nome);
        strcpy(novo->RG, rg);
        novo->idade = idade;
        novo->entrada->dia = entrada.dia;
        novo->entrada->mes = entrada.mes;
        novo->entrada->ano = entrada.ano;
        arvore->raiz = inserir_na_arvore(arvore->raiz, novo, cmp); // Insere o paciente na árvore
        arvore->qtde++;
    }
    fclose(arq);
}

// Funções para comparar pacientes por diferentes critérios
int comparar_por_ano(Celula* a, Celula* b) {
    return a->entrada->ano - b->entrada->ano;
}
int comparar_por_mes(Celula* a, Celula* b) {
    return a->entrada->mes - b->entrada->mes;
}
int comparar_por_dia(Celula* a, Celula* b) {
    return a->entrada->dia - b->entrada->dia;
}
int comparar_por_idade(Celula* a, Celula* b) {
    return a->idade - b->idade;
}

void push_operacao(Pilha* pilha, const char* tipo, Celula* paciente) { // Função para adicionar uma operação na pilha de operações (para desfazer)
    Operacao* nova = malloc(sizeof(Operacao)); // Cria um novo nó de operação
    strcpy(nova->tipo, tipo);// Copia o tipo da operação ("ENFILEIRAR"/"REMOVER")
    nova->paciente = paciente;// Associa o paciente à operação
    nova->prox = pilha->topo;// Aponta para o nó que era o topo anterior
    pilha->topo = nova;// Atualiza o topo da pilha
}

Operacao* pop_operacao(Pilha* pilha){ 
    if(pilha->topo == NULL){// Pilha vazia
       printf("Nao ha registro de operacao na fila");
        return NULL;
    } 
    Operacao* op = pilha->topo; // Guarda o topo (última operação feita)
    pilha->topo = pilha->topo->prox;// Move o topo para o próximo nó
    return op;
}

void mostrar_log(Pilha* pilha) {
    Operacao* atual = pilha->topo;
    printf("\n--- Log de Operações ---\n");
    while (atual != NULL) {
        printf("Operação: %s | Paciente: %s\n", atual->tipo, atual->paciente->nome);
        atual = atual->prox;
    }
}

void desfazer_ultima_operacao(Fila* fila, Pilha* pilha) {
    Operacao* op = pop_operacao(pilha);  // Tenta retirar a última operação da pilha
    if (!op) return;  // Se não houver operação registrada, retorna
    // Pergunta ao usuário se deseja realmente desfazer
    printf("Deseja desfazer a operação: %s (Paciente: %s)? (s/n): ", op->tipo, op->paciente->nome);
    char r;
    scanf(" %c", &r); getchar(); // Lê resposta e consome o '\n' pendente
    if (r != 's' && r != 'S') {
        // Se o usuário não quiser desfazer, apenas libera a memória da operação
        free(op->paciente->entrada);
        free(op->paciente);
        free(op);
        return;
    }
    if (strcmp(op->tipo, "ENFILEIRAR") == 0) { //  Caso o usuário queira desfazer
        if (fila->Qtde > 0) { // Se a última operação foi "enfilerar", desfazer significa remover o paciente da fila
            Efila *rem = fila->Head;
            fila->Head = rem->Proximo;
            if (fila->Head == NULL) fila->Tail = NULL;
            free(rem->Dados->entrada); // Libera o paciente que está no início da fila (último enfileirado)
            free(rem->Dados);
            free(rem);
            fila->Qtde--;
            printf("Desfeita operação de ENFILEIRAR: paciente removido da fila.\n");
        }
    } else if (strcmp(op->tipo, "REMOVER") == 0) {
        // Se a última operação foi "remover", desfazer significa re-inserir o paciente removido
        Efila* novo = inicializa_efila();  // Cria nova célula na fila
        strcpy(novo->Dados->nome, op->paciente->nome); // Copia os dados do paciente de volta para a nova célula
        strcpy(novo->Dados->RG, op->paciente->RG);
        novo->Dados->idade = op->paciente->idade;
        novo->Dados->entrada->dia = op->paciente->entrada->dia;
        novo->Dados->entrada->mes = op->paciente->entrada->mes;
        novo->Dados->entrada->ano = op->paciente->entrada->ano;
        // if (fila->Qtde == 0) { // Insere no final da fila
        //     fila->Head = novo;
        // } else {
        //     fila->Tail->Proximo = novo;
        // }
        // fila->Head->Proximo = 
        Efila* temp = fila->Head;
        fila->Head = novo;
        fila->Head->Proximo = temp;
        fila->Qtde++;
        printf("Desfeita operação de REMOVER: paciente reinserido na fila.\n");
    }
    free(op->paciente->entrada); // Libera a memória da operação e do paciente que estava salvo na pilha
    free(op->paciente);
    free(op);
}

void menu_pesquisa() { // Função para o menu de pesquisa usando árvores binárias
    Arvore arvoreAno = {NULL, 0};
    Arvore arvoreMes = {NULL, 0};
    Arvore arvoreDia = {NULL, 0};
    Arvore arvoreIdade = {NULL, 0};
    inserir_pacientes_em_arvore(&arvoreAno, comparar_por_ano);
    inserir_pacientes_em_arvore(&arvoreMes, comparar_por_mes);
    inserir_pacientes_em_arvore(&arvoreDia, comparar_por_dia);
    inserir_pacientes_em_arvore(&arvoreIdade, comparar_por_idade);
    int op;
    do {
        printf("\n==========================================\n");
        printf("              MENU PESQUISA");
        printf("\n==========================================\n");
        printf("1. Ordenado por ANO\n");
        printf("2. Ordenado por MÊS\n");
        printf("3. Ordenado por DIA\n");
        printf("4. Ordenado por IDADE\n");
        printf("0. Sair\n");
        printf("Escolha: ");
        scanf("%d", &op);
        printf("\n==========================================\n");
        switch (op) {
            case 1: in_order(arvoreAno.raiz); 
                break;
            case 2: in_order(arvoreMes.raiz); 
                break;
            case 3: in_order(arvoreDia.raiz); 
                break;
            case 4: in_order(arvoreIdade.raiz); 
                break;
            case 0: 
                break;
            default: 
            printf("Opção inválida.\n");
        }
    } while (op != 0);
}

void menu_desfazer(Fila *fila, Pilha *pilha) { // Função para o menu de desfazer operações
    char ch, buffer[256];
    while (1) {
        printf("\n==========================================\n");
        printf("          MENU DE OPERACOES (DESFAZER)");
        printf("\n==========================================\n");
        printf("1 - Desfazer ultima operacao\n");
        printf("2 - Mostrar log de operacoes\n");
        printf("0 - Voltar ao menu principal\n");
        printf("==========================================\n");
        printf("Escolha: ");
        fgets(buffer, sizeof(buffer), stdin);
        ch = buffer[0];
        switch (ch) {
            case '1':
                desfazer_ultima_operacao(fila, pilha);
                break;
            case '2':
                mostrar_log(pilha);
                break;
            case '0':
                return;
            default:
                printf("Opção inválida!\n");
        }
    }
}

int menu_fila(Fila *fila,Pilha *pilha) {
    char ch, buffer[256];
    char nome[LEN];
    while (1) {
        printf("\n==========================================\n");
        printf("             MENU DA FILA");
        printf("\n==========================================\n");
        printf("1 - Enfilerar Paciente\n");
        printf("2 - Remover da Fila\n");
        printf("3 - Mostrar Fila\n");
        printf("0 - Voltar Para O Menu Do Sistema\n");
        printf("\n==========================================\n");
        printf("Escolha: ");
        fgets(buffer, sizeof(buffer), stdin);
        ch = buffer[0];
        switch (ch) {
            case '1':
                printf("Digite o nome completo do paciente: ");
                fgets(nome, LEN, stdin);
                limpar_espacos(nome);
                enfilerar(fila, nome,pilha);    
                break;
            case '2':
                remover_da_fila(fila,pilha);
                break;
            case '3':
                Mostra_Fila(fila);
                break;
            case '0':
             return 0;
            default:
                printf("Opção inválida!\n");
        }
    }
}

int menu_fila_prioritario(FilaP *f) { //Função para o Menu da fila prioritaria
    char ch, buffer[256];
    while (1) {
        printf("\n================================================\n");
        printf("             MENU DO ATENDIMENTO PRIORITARIO");
        printf("\n================================================\n");
        printf("1 - Adicionar paciente a fila\n");
        printf("2 - Atender paciente na fila\n");
        printf("3 - Mostrar Fila\n");
        printf("0 - Voltar Para O Menu Do Sistema\n");
        printf("\n================================================\n");
        printf("Escolha: ");
        fgets(buffer, sizeof(buffer), stdin);
        ch = buffer[0];
        switch (ch) {
            case '1':
                adicionar_na_heap(f);   
                break;
            case '2':
                remover_maior_prioridade(f);
                break;
            case '3':
                mostrar_heap(f);
                break;
            case '0':
             return 0;
            default:
                printf("Opção inválida!\n");
        }
    }
}

void menu(Lista* l, Fila* f, FilaP *h,Pilha *p){ //Função para o Menu do Sitstema
    char ch, buffer[256];
    while (1) {
        printf("\n==========================================\n");
        printf("             MENU DO SISTEMA");
        printf("\n==========================================\n");
        printf("1 - Cadastro\n");
        printf("2 - Fila De Atendimento\n");
        printf("3 - Fila De Atendimento Prioritario\n");
        printf("4 - Pesquisar\n");
        printf("5 - Desfazer\n");
        printf("6 - Salvar arquivo\n");
        printf("7 - Sobre\n");
        printf("0 - Sair\n");
        printf("\n==========================================\n");
        printf("Escolha: ");
        fgets(buffer, sizeof(buffer), stdin);
        ch = buffer[0];
        switch (ch) {
            case '1':
                menu_cadastro(l);
                break;
            case '2':
                menu_fila(f,p);
                break;
            case '3': 
                menu_fila_prioritario(h);
                break; 
            case '4':
                menu_pesquisa();
                break;
            case '5': 
                menu_desfazer(f,p);
                break; 
            case '6':
                printf("Arquivo salvo com sucesso!\n");
                break;
            case '7':
                Sobre();
                break;
            case '0':
                printf("Adeus!\n");
                liberar_fila(f);
                liberar_lista(l);
                return;
            default:
                printf("Opção inválida! \n");
        }
    }
}

int main() { // Função principal
    FilaP filap;
    filap.qtde = 0;
    Lista *lista = inicializa_lista();
    Fila *fila = inicializa_fila(); 
    Pilha *pilha = inicializa_pilha(); 
    ler_pacientes(lista);
    menu(lista, fila, &filap,pilha); // Chama o menu principal do sistema
    return 0;
}