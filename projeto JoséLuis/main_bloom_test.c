/*
 * main_bloom_test.c
 * ------------------
 * Programa de demonstracao/teste APENAS da Parte 2 (Filtro de Bloom),
 * conforme pedido. Mais tarde isso deve ser integrado ao main.c do
 * projeto completo (junto com a Tabela Hash da Parte 1).
 *
 * O que este teste faz:
 *   1. Cria um filtro de Bloom dimensionado para 1000 elementos
 *      com taxa de falso positivo desejada de 1%.
 *   2. Insere 1000 strings conhecidas (ex: "usuario0" a "usuario999").
 *   3. Consulta todas as 1000 strings inseridas -> devem retornar "existe".
 *   4. Consulta 1000 strings que NUNCA foram inseridas -> mede quantas
 *      o filtro erroneamente diz que "existem" (falsos positivos).
 *   5. Imprime estatisticas de dimensionamento e taxa de falso positivo
 *      teorica vs. taxa medida na pratica.
 */

#include <stdio.h>
#include <string.h>
#include "bloom.h"

int main(void) {
    size_t N = 1000;
    double FP_RATE = 0.01; /* 1% de falso positivo desejado */

    BloomFilter *bf = bloom_create(N, FP_RATE);
    if (!bf) {
        fprintf(stderr, "Erro ao criar o filtro de Bloom.\n");
        return 1;
    }

    printf("Filtro criado para n=%zu elementos, p desejado=%.2f%%\n\n", N, FP_RATE * 100.0);

    /* 1) Inserir N elementos conhecidos */
    char buffer[32];
    for (size_t i = 0; i < N; i++) {
        snprintf(buffer, sizeof(buffer), "usuario%zu", i);
        bloom_insert(bf, buffer);
    }

    /* 2) Consultar os mesmos elementos inseridos (devem TODOS existir) */
    size_t encontrados = 0;
    for (size_t i = 0; i < N; i++) {
        snprintf(buffer, sizeof(buffer), "usuario%zu", i);
        if (bloom_query(bf, buffer)) {
            encontrados++;
        }
    }
    printf("Elementos inseridos corretamente identificados como existentes: %zu / %zu\n", encontrados, N);
    if (encontrados != N) {
        printf("ATENCAO: isso nunca deveria acontecer (Bloom nao gera falso negativo)!\n");
    }

    /* 3) Consultar elementos que NUNCA foram inseridos -> medir falsos positivos */
    size_t total_testes_negativos = N;
    size_t falsos_positivos = 0;
    for (size_t i = 0; i < total_testes_negativos; i++) {
        /* prefixo diferente garante que nunca foi inserido */
        snprintf(buffer, sizeof(buffer), "NAOEXISTE%zu", i);
        if (bloom_query(bf, buffer)) {
            falsos_positivos++;
        }
    }

    double taxa_medida = (double)falsos_positivos / (double)total_testes_negativos;

    printf("\nTeste com %zu elementos NAO inseridos:\n", total_testes_negativos);
    printf("Falsos positivos detectados: %zu\n", falsos_positivos);
    printf("Taxa de falso positivo MEDIDA: %.4f%%\n\n", taxa_medida * 100.0);

    bloom_print_stats(bf);

    bloom_destroy(bf);
    return 0;
}
