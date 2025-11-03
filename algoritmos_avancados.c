#include <stdio.h> // I/O padrão: printf, scanf, etc.
#include <stdlib.h> // malloc, free, exit, system
#include <string.h> // funções de string: strlen, strcmp, strncpy
#include <locale.h> // Configuração de idioma e acentuação
#include <ctype.h> // toupper, tolower
#ifdef _WIN32
    #include <windows.h> // Para SetConsoleCP/SetConsoleOutputCP e Sleep
#else
    #include <unistd.h> // Para sleep em sistemas UNIX/Linux
#endif

// CONSTATNTES

#define TAM_HASH 11  // Tamanho da tabela hash (pequeno para didática)
#define MAX_PISTA 80 // Máximo de caracteres para texto de pista
#define MAX_NOME 50 // Máximo de caracteres para nomes (salas/suspeitos)

// ESTRUTURAS

//Estrutura de cada cômodo da mansãos
typedef struct Sala {
    char nome[MAX_NOME];
    char pista[MAX_PISTA]; // nova linha
    struct Sala* esquerda;
    struct Sala* direita;
} Sala;

// Nó da BST para pistas
typedef struct PistaNode {
    char texto[MAX_PISTA];
    struct PistaNode* esquerda;
    struct PistaNode* direita;
} PistaNode;

// Nó da lista encadeada armazenada dentro da tabela hash
typedef struct SuspeitoNode {
    char nome[MAX_NOME];
    char pista[MAX_PISTA];
    struct SuspeitoNode* prox;
} SuspeitoNode;

SuspeitoNode* tabelaHash[TAM_HASH] = {NULL}; // Array de ponteiros para listas encadeadas

//PROTOTIPOS

void limpaTela(); // Limpa o terminal (portável)
void limpaBufferEntrada(); // Limpa buffer do teclado
void pausa(int ms);

Sala* criarSala(const char* nome, const char* pista); // Criação dinâmica de sala
void explorarSalasComPistas(Sala* atual, PistaNode** arvorePistas); // Navegação + coleta

PistaNode* inserirPista(PistaNode* raiz, const char* texto); // Insere pista na BST
void exibirPistas(PistaNode* raiz); // Exibe BST em ordem
void liberarPistas(PistaNode* raiz); // Libera memória da BST

void inicializarHash(); // Inicializa tabela hash (preenche com NULL)
int funcaoHash(const char* nome); // Calcula índice a partir de uma chave (string)
void listarSuspeitos(); // Imprime todas as associações na hash
void suspeitoMaisProvavel(); // Determina quem possui mais pistas associadas
void liberarHash(); // Libera memória usada pela tabela hash
void liberarSalas(Sala* raiz); // Libera recursivamente a árvore de salas
void inserirSuspeitoHash(const char* pista, const char* suspeito); // Insere na hash


const char* pegarSuspeitoParaPista(const char* pista); // Mapeamento didático pista->suspeito
void relacionarPistasASuspeitos(PistaNode* raiz); // Percorre BST e popula a hash

// FUNÇÃO PRINCIPAL ( MAIN )

int main() {

    // Define locale para acentuação em português
    setlocale(LC_ALL, "pt_BR.UTF-8");
#ifdef _WIN32
    SetConsoleCP(CP_UTF8);          
    SetConsoleOutputCP(CP_UTF8);    
#endif

    limpaTela(); // Limpa tela antes de iniciar a execução

    //Montagem do mapa da mansão (árvore fixa)
    Sala* hallEntrada = criarSala("Hall de Entrada", "Pegadas estranhas");
    Sala* salaEstar = criarSala("Sala de Estar", "Copo quebrado no chão");
    Sala* biblioteca = criarSala("Biblioteca", "Livro antigo fora do lugar");
    Sala* cozinha = criarSala("Cozinha", "Faca faltando no suporte");
    Sala* jardim = criarSala("Jardim", "Terra revirada");
    Sala* sotao = criarSala("Sótão", "Baú com cadeado arrombado");
    
    // Conexões da árvore (definição dos caminhos)
    hallEntrada->esquerda = salaEstar;
    hallEntrada->direita  = biblioteca;

    salaEstar->esquerda = cozinha;
    salaEstar->direita  = jardim;

    biblioteca->direita = sotao;

    // MENU PRINCIPAL
    PistaNode* arvorePistas = NULL; // BST vazia
    printf("=====================================================\n");
    printf("    DESAFIO DETETIVE QUEST - MANSÃO - NIVEL MESTRE\n");
    printf("=====================================================\n");
    
    inicializarHash(); // zera a tabela hash (prepara para inserções)
    explorarSalasComPistas(hallEntrada, &arvorePistas); // inicia exploração a partir do Hall
    
    char opcao; {
    printf("\nDeseja visualizar as pistas coletadas? (S/N): ");
    scanf(" %c", &opcao); // lê uma opção char (ignora espaços em branco anteriores)
    limpaBufferEntrada(); // limpa eventual resto no buffer

        if (opcao == 's' || opcao == 'S') {
            printf("\n======= PISTAS COLETADAS =======\n");
            exibirPistas(arvorePistas);
            
        } else {
        printf("\nOk! Fim da exploração.\n");

        }

        relacionarPistasASuspeitos(arvorePistas); // percorre BST
        
        listarSuspeitos(); // imprime todas as associações pista→suspeito
        
        suspeitoMaisProvavel(); // faz a análise e imprime o suspeito mais citado

        system("pause");
        printf("\nFim de Jogo\n\nLiberando Memoria!!!!");
        pausa(2000);
        limpaTela();
    }

    // Liberação da memória alocada dinamicamente
    liberarPistas(arvorePistas); // libera BST de pistas
    liberarHash(); // libera lista(s) da tabela hash
    liberarSalas(hallEntrada); // libera árvore de salas (recursivamente)

    return 0;
}

// FUNÇÕES UTILITARIAS 

// Limpa a tela 
void limpaTela() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Limpa buffer do teclado para evitar caracteres residuais
void limpaBufferEntrada() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

void pausa(int ms) {
#ifdef _WIN32
    Sleep(ms);
#else
    usleep(ms * 1000); // usleep trabalha em microssegundos
#endif
}

//Cria dinamicamente uma sala com nome e sem conexões
Sala* criarSala(const char* nome, const char* pista) {
    Sala* novaSala = (Sala*) malloc(sizeof(Sala)); // aloca memória para o nó Sala
    if (!novaSala) {
        printf("Erro ao alocar memória!\n");
        exit(1);
    }
    strncpy(novaSala->nome, nome, MAX_NOME -1); // copia o nome com proteção contra overflow e garante terminação nula
    novaSala->nome[MAX_NOME -1] = '\0';
    // se pista não for NULL, copia com proteção; caso contrário marca como string vazia
    if (pista != NULL) { 
        strncpy(novaSala->pista, pista, MAX_PISTA -1);
        novaSala->pista[MAX_PISTA -1] = '\0';
    } else {
        novaSala->pista[0] = '\0';
    }
    // inicializa ponteiros filhos
    novaSala->esquerda = NULL;
    novaSala->direita = NULL;
    return novaSala; // retorna ponteiro para sala criada
}

//Navegação interativa pela árvore 

// Esta função percorre a árvore de salas
void explorarSalasComPistas(Sala* atual, PistaNode** arvorePistas) {
    char escolha;

    // continua enquanto houver sala atual
    while (atual != NULL) {
        printf("\nVocê está em: %s\n", atual->nome);

       // coleta automática de pista
        if (strlen(atual->pista) > 0) {
            printf("\nVocê encontrou uma pista: %s\n", atual->pista);
            *arvorePistas = inserirPista(*arvorePistas, atual->pista);
            printf("\n=====================================================");
    
        }

        printf("\nEscolha seu caminho:\n");
        printf("(E) Esquerda\n");
        printf("(D) Direita\n");
        printf("(S) Sair do jogo\n");
        printf("=====================================================\n");
        printf("Sua opção: ");
        if (scanf(" %c", &escolha) != 1) { // lê escolha do usuário
            limpaBufferEntrada();
            printf("\nEntrada inválida. Tente novamente.\n");
            continue;
        }

        limpaBufferEntrada();

        if (escolha == 'e' || escolha == 'E') {
            if (atual->esquerda != NULL)
                atual = atual->esquerda; // move para filho esquerdo
            else
                printf("\nCaminho para a ESQUERDA inexistente!\n");
        }
        else if (escolha == 'd' || escolha == 'D') {
            if (atual->direita != NULL)
                atual = atual->direita; // move para filho direito
            else
                printf("\nCaminho para a DIREITA inexistente!\n");
        }
        else if (escolha == 's' || escolha == 'S') {
            printf("\nExploração encerrada pelo jogador!\n");
            pausa(2000);
            return;
        }
        else {
            printf("\nOpção inválida! Tente Novamente!\n");
        }
           
    }
}

// ÁRVORE BST DE PISTAS

PistaNode* inserirPista(PistaNode* raiz, const char* texto) {
    if (raiz == NULL) {
        PistaNode* novo = (PistaNode*) malloc(sizeof(PistaNode));
        if (!novo) {
            printf("Erro ao alocar memória para pista!\n");
            exit(1);
        }
        strncpy(novo->texto, texto, MAX_PISTA -1);
        novo->texto[MAX_PISTA - 1] = '\0';
        novo->esquerda = NULL;
        novo->direita = NULL;
        return novo;
    }
    int cmp = strcmp(texto, raiz->texto); // compara strings
    if (cmp < 0)
        raiz->esquerda = inserirPista(raiz->esquerda, texto);
    else if (cmp > 0)
        raiz->direita = inserirPista(raiz->direita, texto);
    // duplicados são ignorados
    return raiz;
}

void exibirPistas(PistaNode* raiz) {
    if (raiz == NULL) return;
    exibirPistas(raiz->esquerda);
    printf("%s\n\n", raiz->texto);
    exibirPistas(raiz->direita);
}

void liberarPistas(PistaNode* raiz) {
    if (raiz == NULL) return;
    liberarPistas(raiz->esquerda);
    liberarPistas(raiz->direita);
    free(raiz);
}

void liberarSalas(Sala* raiz) {
    if (raiz == NULL) return;
    liberarSalas(raiz->esquerda);
    liberarSalas(raiz->direita);
    free(raiz);
}

// TABELA HASH (SUSPEITOS)

// Inicializa tabela hash com NULLs
void inicializarHash() {
    for (int i = 0; i < TAM_HASH; i++)
        tabelaHash[i] = NULL;
}

// Função hash simples: soma ASCII e modulo TAM_HASH 
int funcaoHash(const char* chave) {
    int soma = 0;
    for (size_t i = 0; i < strlen(chave); i++) {
        soma += (unsigned char) chave[i];
    }
    return (soma % TAM_HASH);
}

// Insere uma relação pista -> suspeito na tabela hash ( encadeamento )
void inserirSuspeitoHash(const char* pista, const char* suspeito) {
    int indice = funcaoHash(suspeito); // calcula índice a partir do nome do suspeito

    SuspeitoNode* novo = (SuspeitoNode*) malloc(sizeof(SuspeitoNode));
    if (!novo) {
        printf("Erro ao alocar memória para suspeito!\n");
        exit(1);
    }
    // copia dados com proteção contra overflow
    strncpy(novo->nome, suspeito, MAX_NOME -1);
    novo->nome[MAX_NOME -1] = '\0';
    strncpy(novo->pista, pista, MAX_PISTA -1 );
    novo->pista[MAX_PISTA - 1] = '\0';
    
    // insere no início da lista (tratamento simples de colisões)
    novo->prox = tabelaHash[indice];
    tabelaHash[indice] = novo;
}

// Lista todas as associações suspeito -> pista
void listarSuspeitos() {
    printf("\n=========== Suspeitos e Pistas Associadas ==========\n");
    int encontrou = 0;
    for (int i = 0; i < TAM_HASH; i++) {
        SuspeitoNode* atual = tabelaHash[i];
        while (atual) {
            printf("Suspeito: %s => Pista: %s\n", atual->nome, atual->pista);
            atual = atual->prox;
            encontrou = 1;
        }
    }
    if (!encontrou) {
        printf("Nenhuma associação de pistas a suspeitos foi registrada.\n");
    }
}

// Libera toda a tabela hash
void liberarHash() {
    for (int i = 0; i < TAM_HASH; i++) {
        SuspeitoNode* atual = tabelaHash[i];
        while (atual) {
            SuspeitoNode* aux = atual->prox;
            free(atual);
            atual = aux;
        }
        tabelaHash[i] = NULL;
    }
}

// Calcula qual suspeito aparece mais vezes na tabela (mais pistas associadas)
void suspeitoMaisProvavel() {
    char candidato[MAX_NOME] = "";
    int maior = 0;

    printf("\n============= Analisando Evidências... =============\n");
    // conta quantos nós totais existem
    int totalNos = 0;
    for (int i = 0; i < TAM_HASH; i++) {
        SuspeitoNode* atual = tabelaHash[i];
        while (atual) { 
            totalNos++; 
            atual = atual->prox; 
        }
    }

    if (totalNos == 0) {
        printf("\nNenhuma pista relacionada a suspeitos para analisar.\n");
        return;
    }
    // para cada nó, conta quantas ocorrências do mesmo nome existem
    for (int i = 0; i < TAM_HASH; i++) {
        SuspeitoNode* atual = tabelaHash[i];
        while (atual) {
            int contador = 0;
            // percorre toda a tabela para contar aparições do mesmo nome
            for (int j = 0; j < TAM_HASH; j++) {
                SuspeitoNode* aux = tabelaHash[j];
                while (aux) {
                    if (strcmp(aux->nome, atual->nome) == 0) contador++;
                    aux = aux->prox;
                }
            }
            // atualiza candidato se esse nome tiver mais ocorrências
            if (contador > maior) {
                maior = contador;
                strncpy(candidato, atual->nome, MAX_NOME);                
            }
            atual = atual->prox;
        }
    }

    printf("\n=============== Resultado da Análise ===============\n");
    printf("Principal suspeito: %s (%d pista(s) ligada(s))\n", candidato, maior);
    printf("=====================================================\n\n");       
}

// MAPEAMENTO 

// Retorna o nome do suspeito associado a uma pista
// Aqui definimos manualmente as relações
const char* pegarSuspeitoParaPista(const char* pista) {
    if (strcmp(pista, "Pegadas estranhas") == 0) return "Caseiro Bruno";
    if (strcmp(pista, "Copo quebrado no chão") == 0) return "Mordomo Carlos";
    if (strcmp(pista, "Livro antigo fora do lugar") == 0) return "Bibliotecaria Júlia";
    if (strcmp(pista, "Faca faltando no suporte") == 0) return "Chef Ana";
    if (strcmp(pista, "Terra revirada") == 0) return "Jardineiro Leo";
    if (strcmp(pista, "Baú com cadeado arrombado") == 0) return "Ex-Morador Diego";

    // Padrão se nenhuma associação predefinida
    return "Desconhecido";
}

// Percorre a árvore de pistas coletadas e insere suas associações na hash
void relacionarPistasASuspeitos(PistaNode* raiz) {
    if (raiz == NULL) return;
    // percorrer em ordem (ou qualquer ordem)
    relacionarPistasASuspeitos(raiz->esquerda); // visita esquerda (in-order)

    const char* suspeito = pegarSuspeitoParaPista(raiz->texto);
    if (suspeito != NULL) {
        inserirSuspeitoHash(raiz->texto, suspeito);
    }

    relacionarPistasASuspeitos(raiz->direita);
}


// Desafio Detective Quest
// Tema 4 - Árvores e Tabela Hash
// Este código inicial serve como base para o desenvolvimento das estruturas de navegação, pistas e suspeitos.
// Use as instruções de cada região para desenvolver o sistema completo com árvore binária, árvore de busca e tabela hash.

// 🌱 Nível Novato: Mapa da Mansão com Árvore Binária
//
// - Crie uma struct Sala com nome, e dois ponteiros: esquerda e direita.
// - Use funções como criarSala(), conectarSalas() e explorarSalas().
// - A árvore pode ser fixa: Hall de Entrada, Biblioteca, Cozinha, Sótão etc.
// - O jogador deve poder explorar indo à esquerda (e) ou à direita (d).
// - Finalize a exploração com uma opção de saída (s).
// - Exiba o nome da sala a cada movimento.
// - Use recursão ou laços para caminhar pela árvore.
// - Nenhuma inserção dinâmica é necessária neste nível.

// 🔍 Nível Aventureiro: Armazenamento de Pistas com Árvore de Busca
//
// - Crie uma struct Pista com campo texto (string).
// - Crie uma árvore binária de busca (BST) para inserir as pistas coletadas.
// - Ao visitar salas específicas, adicione pistas automaticamente com inserirBST().
// - Implemente uma função para exibir as pistas em ordem alfabética (emOrdem()).
// - Utilize alocação dinâmica e comparação de strings (strcmp) para organizar.
// - Não precisa remover ou balancear a árvore.
// - Use funções para modularizar: inserirPista(), listarPistas().
// - A árvore de pistas deve ser exibida quando o jogador quiser revisar evidências.

// 🧠 Nível Mestre: Relacionamento de Pistas com Suspeitos via Hash
//
// - Crie uma struct Suspeito contendo nome e lista de pistas associadas.
// - Crie uma tabela hash (ex: array de ponteiros para listas encadeadas).
// - A chave pode ser o nome do suspeito ou derivada das pistas.
// - Implemente uma função inserirHash(pista, suspeito) para registrar relações.
// - Crie uma função para mostrar todos os suspeitos e suas respectivas pistas.
// - Adicione um contador para saber qual suspeito foi mais citado.
// - Exiba ao final o “suspeito mais provável” baseado nas pistas coletadas.
// - Para hashing simples, pode usar soma dos valores ASCII do nome ou primeira letra.
// - Em caso de colisão, use lista encadeada para tratar.
// - Modularize com funções como inicializarHash(), buscarSuspeito(), listarAssociacoes().
