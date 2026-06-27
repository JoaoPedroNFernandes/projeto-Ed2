#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "hash.h"
#include "bloom.h"

#define TAMANHO_HASH 131071
#define BLOOM_EXPECTED_N  100000
#define BLOOM_FP_RATE     0.01

static int quantidadeUsuarios  = 0;
static int consultas           = 0;
static int consultas_sem_bloom = 0;
static int consultasEvitadas   = 0;
static int consultasPositivas  = 0;
static int falsoPositivo       = 0;
static double tempoTotal       = 0.0;
static double tempoTotal2      = 0.0;

void inserirUsuario(TabelaHash *hash, BloomFilter *bloom, const char *usuario) {
    bloom_insert(bloom, usuario);
    hash_inserir(hash, usuario);
    quantidadeUsuarios++;
}

void consultaUsuario(TabelaHash *hash, BloomFilter *bloom, const char *usuario) {
    consultas++;
    clock_t inicio = clock();

    if (!bloom_query(bloom, usuario)) {
        printf("Bloom: definitivamente nao existe. Consulta a hash evitada.\n");
        consultasEvitadas++;
    } else {
        if (hash_buscar(hash, usuario)) {
            printf("Usuario '%s' encontrado.\n", usuario);
            consultasPositivas++;
        } else {
            printf("Usuario '%s' nao encontrado (falso positivo do Bloom).\n", usuario);
            falsoPositivo++;
        }
    }

    tempoTotal += (double)(clock() - inicio) / CLOCKS_PER_SEC;
}

void consultaUsuarioSemBloom(TabelaHash *hash, const char *usuario) {
    consultas_sem_bloom++;
    clock_t inicio = clock();

    if (hash_buscar(hash, usuario))
        printf("Usuario '%s' encontrado.\n", usuario);
    else
        printf("Usuario '%s' nao encontrado.\n", usuario);

    tempoTotal2 += (double)(clock() - inicio) / CLOCKS_PER_SEC;
}

void mostrarEstatisticas(const TabelaHash *hash, const BloomFilter *bloom) {
    printf("\n===== Estatisticas (modo interativo) =====\n");
    printf("Elementos armazenados          : %d\n", quantidadeUsuarios);
    printf("Consultas com Bloom            : %d\n", consultas);
    printf("Consultas evitadas (Bloom=nao) : %d\n", consultasEvitadas);
    printf("Usuarios encontrados           : %d\n", consultasPositivas);
    printf("Falsos positivos               : %d\n", falsoPositivo);

    if (consultas > 0) {
        printf("Taxa de falsos positivos       : %.2f%%\n",
               100.0 * falsoPositivo / consultas);
        printf("Tempo medio com Bloom          : %.9f s\n",
               tempoTotal / consultas);
    }
    printf("Tempo total com Bloom          : %.6f s\n", tempoTotal);

    printf("Consultas sem Bloom            : %d\n", consultas_sem_bloom);
    if (consultas_sem_bloom > 0) {
        printf("Tempo medio sem Bloom          : %.9f s\n",
               tempoTotal2 / consultas_sem_bloom);
    }
    printf("Tempo total sem Bloom          : %.6f s\n", tempoTotal2);
    printf("\n");
    hash_imprimir_estatisticas(hash);
    printf("\n");
    bloom_print_stats(bloom);
}

int inserirArquivo(TabelaHash *hash, BloomFilter *bloom, const char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir arquivo '%s'.\n", nomeArquivo);
        return 0;
    }

    char usuario[16];
    int count = 0;
    while (fgets(usuario, sizeof(usuario), arquivo)) {
        usuario[strcspn(usuario, "\n\r")] = '\0';
        if (usuario[0] != '\0') {
            inserirUsuario(hash, bloom, usuario);
            count++;
        }
    }
    fclose(arquivo);
    printf("Carregados %d usuarios de '%s'.\n", count, nomeArquivo);
    return count;
}

static int contarLinhas(const char *nomeArquivo) {
    FILE *f = fopen(nomeArquivo, "r");
    if (!f) return 0;
    char buf[16];
    int n = 0;
    while (fgets(buf, sizeof(buf), f)) {
        buf[strcspn(buf, "\n\r")] = '\0';
        if (buf[0] != '\0') n++;
    }
    fclose(f);
    return n;
}

static void gerarNomeAleatorio(char *buf) {
    for (int i = 0; i < 8; i++)
        buf[i] = 'a' + (rand() % 26);
    for (int i = 8; i < 11; i++)
        buf[i] = '0' + (rand() % 10);
    buf[11] = '\0';
}

void executarExperimento(const char *nomeArquivo) {
    printf("\n--- Experimento: '%s' ---\n", nomeArquivo);

    int n = contarLinhas(nomeArquivo);
    if (n == 0) {
        printf("Arquivo vazio ou inexistente.\n");
        return;
    }

    TabelaHash *hash = hash_criar(TAMANHO_HASH);
    if (!hash) { printf("Erro ao alocar tabela hash.\n"); return; }

    BloomFilter *bloom = bloom_create((size_t)n, 0.01);
    if (!bloom) { hash_destruir(hash); printf("Erro ao criar Bloom.\n"); return; }

    FILE *arquivo = fopen(nomeArquivo, "r");
    if (!arquivo) {
        bloom_destroy(bloom);
        hash_destruir(hash);
        return;
    }
    char linha[16];
    int carregados = 0;
    while (fgets(linha, sizeof(linha), arquivo)) {
        linha[strcspn(linha, "\n\r")] = '\0';
        if (linha[0] != '\0') {
            bloom_insert(bloom, linha);
            hash_inserir(hash, linha);
            carregados++;
        }
    }
    fclose(arquivo);
    printf("Elementos carregados: %d\n", carregados);

    arquivo = fopen(nomeArquivo, "r");
    clock_t t0 = clock();
    while (arquivo && fgets(linha, sizeof(linha), arquivo)) {
        linha[strcspn(linha, "\n\r")] = '\0';
        if (linha[0] != '\0')
            hash_buscar(hash, linha);
    }
    double tempo_sem_bloom = (double)(clock() - t0) / CLOCKS_PER_SEC;
    if (arquivo) fclose(arquivo);

    arquivo = fopen(nomeArquivo, "r");
    t0 = clock();
    while (arquivo && fgets(linha, sizeof(linha), arquivo)) {
        linha[strcspn(linha, "\n\r")] = '\0';
        if (linha[0] == '\0') continue;
        if (bloom_query(bloom, linha))
            hash_buscar(hash, linha);
    }
    double tempo_com_bloom = (double)(clock() - t0) / CLOCKS_PER_SEC;
    if (arquivo) fclose(arquivo);

    int testes = carregados;
    int fp = 0;
    srand((unsigned int)time(NULL));
    for (int i = 0; i < testes; i++) {
        char nome[12];
        gerarNomeAleatorio(nome);
        if (bloom_query(bloom, nome) && !hash_buscar(hash, nome))
            fp++;
    }
    double taxa_fp = 100.0 * fp / testes;

    printf("\n%-12s  %-16s  %-15s  %-18s\n",
           "Elementos", "T. sem Bloom (s)", "T. com Bloom (s)", "Falsos Positivos");
    printf("%-12d  %-16.6f  %-15.6f  %.2f%%\n",
           carregados, tempo_sem_bloom, tempo_com_bloom, taxa_fp);
    printf("(FP: %d em %d consultas aleatorias)\n", fp, testes);

    printf("\n");
    hash_imprimir_estatisticas(hash);
    printf("\n");
    bloom_print_stats(bloom);

    bloom_destroy(bloom);
    hash_destruir(hash);
    printf("--- Fim do experimento ---\n");
}

int main(void) {
    TabelaHash *hash = hash_criar(TAMANHO_HASH);
    if (!hash) { fprintf(stderr, "Erro ao criar tabela hash.\n"); return 1; }

    BloomFilter *bloom = bloom_create(BLOOM_EXPECTED_N, BLOOM_FP_RATE);
    if (!bloom) { hash_destruir(hash); fprintf(stderr, "Erro ao criar Bloom.\n"); return 1; }

    int opcao;
    char usuario[13];
    char nomeArquivo[256];

    do {
        printf("\n========================================\n");
        printf("  Sistema de Verificacao de Cadastro\n");
        printf("========================================\n");
        printf("1. Inserir usuario\n");
        printf("2. Consultar (com Bloom Filter)\n");
        printf("3. Consultar (sem Bloom Filter)\n");
        printf("4. Inserir em lote (arquivo)\n");
        printf("5. Estatisticas\n");
        printf("6. Executar experimento\n");
        printf("0. Sair\n");
        printf("Opcao: ");

        if (scanf("%d", &opcao) != 1) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            opcao = -1;
            continue;
        }

        switch (opcao) {
            case 1:
                printf("Nome do usuario (ate 11 chars): ");
                (void)scanf("%11s", usuario);
                inserirUsuario(hash, bloom, usuario);
                printf("Usuario '%s' inserido.\n", usuario);
                break;
            case 2:
                printf("Nome do usuario: ");
                (void)scanf("%11s", usuario);
                consultaUsuario(hash, bloom, usuario);
                break;
            case 3:
                printf("Nome do usuario: ");
                (void)scanf("%11s", usuario);
                consultaUsuarioSemBloom(hash, usuario);
                break;
            case 4:
                printf("Caminho do arquivo: ");
                (void)scanf("%255s", nomeArquivo);
                inserirArquivo(hash, bloom, nomeArquivo);
                break;
            case 5:
                mostrarEstatisticas(hash, bloom);
                break;
            case 6:
                printf("Caminho do arquivo para experimento: ");
                (void)scanf("%255s", nomeArquivo);
                executarExperimento(nomeArquivo);
                break;
            case 0:
                printf("Encerrando...\n");
                break;
            default:
                printf("Opcao invalida.\n");
        }
    } while (opcao != 0);

    bloom_destroy(bloom);
    hash_destruir(hash);
    return 0;
}
