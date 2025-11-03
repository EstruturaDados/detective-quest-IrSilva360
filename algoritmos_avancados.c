#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h> // Configuração de idioma e acentuação

#ifdef _WIN32
    #include <windows.h> // Para SetConsoleCP/SetConsoleOutputCP e Sleep
#else
    #include <unistd.h> // Para sleep em sistemas UNIX/Linux
#endif

void limpaTela(); // Limpa o terminal (portável)
void limpaBufferEntrada(); // Limpa buffer do teclado
void pausa();
void explorarSalas();

//Estrutura de cada cômodo da mansão
typedef struct Sala {
    char nome[50];           // Nome da sala
    struct Sala* esquerda;   // Caminho à esquerda
    struct Sala* direita;    // Caminho à direita
} Sala;

//Cria dinamicamente uma sala com nome e sem conexões
Sala* criarSala(const char* nome) {
    Sala* novaSala = (Sala*) malloc(sizeof(Sala));
    if (!novaSala) {
        printf("Erro ao alocar memória!\n");
        exit(1);
    }
    strcpy(novaSala->nome, nome);
    novaSala->esquerda = NULL;
    novaSala->direita = NULL;
    return novaSala;
}

int main() {

    setlocale(LC_ALL, "pt_BR.UTF-8");
#ifdef _WIN32
    SetConsoleCP(CP_UTF8);          
    SetConsoleOutputCP(CP_UTF8);    
#endif

    limpaTela();
    //Montagem do mapa da mansão (árvore fixa)

    Sala* hallEntrada = criarSala("Hall de Entrada");
    Sala* salaEstar   = criarSala("Sala de Estar");
    Sala* biblioteca  = criarSala("Biblioteca");
    Sala* cozinha     = criarSala("Cozinha");
    Sala* jardim      = criarSala("Jardim");
    Sala* sotao       = criarSala("Sótão");

    // Conexões da árvore (definição dos caminhos)
    hallEntrada->esquerda = salaEstar;
    hallEntrada->direita  = biblioteca;

    salaEstar->esquerda = cozinha;
    salaEstar->direita  = jardim;

    biblioteca->direita = sotao;

    //Início da exploração
    printf("================================================\n");
    printf(" DESAFIO DETETIVE QUEST - MANSÃO - NIVEL NOVATO\n");
    printf("================================================\n");
    explorarSalas(hallEntrada);

    // Liberação da memória (boa prática)
    free(sotao);
    free(jardim);
    free(cozinha);
    free(biblioteca);
    free(salaEstar);
    free(hallEntrada);

    return 0;
}

// Utilitários 

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

//Navegação interativa pela árvore
void explorarSalas(Sala* atual) {
    char escolha;

    while (atual != NULL) {
        printf("\nVocê está em: %s\n", atual->nome);

        // Se for um nó-folha, terminou o caminho
        if (atual->esquerda == NULL && atual->direita == NULL) {
            printf("\nNão há mais caminhos! Fim da exploração!\n");
            pausa(1500);
            printf("\nLiberando Memoria!!!!");
            pausa(1000);
            limpaTela();
            return;
        }

        printf("\nEscolha seu caminho:\n");
        printf("(E) Esquerda\n");
        printf("(D) Direita\n");
        printf("(S) Sair do jogo\n");
        printf("Sua opção: ");
        scanf(" %c", &escolha);
        limpaBufferEntrada();

        if (escolha == 'e' || escolha == 'E') {
            if (atual->esquerda != NULL)
                atual = atual->esquerda;
            else
                printf("\nCaminho para a ESQUERDA inexistente!\n");
        }
        else if (escolha == 'd' || escolha == 'D') {
            if (atual->direita != NULL)
                atual = atual->direita;
            else
                printf("\nCaminho para a DIREITA inexistente!\n");
        }
        else if (escolha == 's' || escolha == 'S') {
            printf("\nExploração encerrada pelo jogador!\n");
            pausa(1500);
            printf("\nLiberando Memoria!!!!");
            pausa(1500);
            limpaTela();
            return;
        }
        else {
            printf("\nOpção inválida! Tente Novamente!\n");
        }
    }
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
