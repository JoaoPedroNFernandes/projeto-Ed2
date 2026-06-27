/*
 * bloom.h
 * --------
 * Interface do Filtro de Bloom (estrutura probabilística de pertinência).
 *
 * Um Filtro de Bloom NUNCA gera falsos negativos:
 *   - se ele disser "não existe", o elemento DEFINITIVAMENTE não foi inserido.
 *   - se ele disser "existe", o elemento PODE ter sido inserido (falso positivo
 *     é possível e seu percentual é controlado pelo dimensionamento).
 *
 * Por isso, no fluxo do trabalho, o Bloom é consultado ANTES da Tabela Hash:
 * se ele já garante a ausência, evita-se uma busca cara na Hash.
 */

#ifndef BLOOM_H
#define BLOOM_H

#include <stddef.h>

/* Estrutura do Filtro de Bloom */
typedef struct {
    unsigned char *bits;   /* vetor de bits, armazenado como array de bytes   */
    size_t m;               /* tamanho do vetor de bits (em bits)              */
    int k;                   /* quantidade de funções hash usadas               */
    size_t n;               /* quantidade de elementos inseridos até o momento */
    size_t expected_n;      /* quantidade de elementos esperada (dimensionamento) */
    double target_fp_rate;  /* taxa de falso positivo desejada (ex: 0.01 = 1%) */
} BloomFilter;

/*
 * bloom_create
 * ------------
 * Cria e dimensiona um filtro de Bloom de forma adequada, a partir de:
 *   - expected_elements: quantidade esperada de elementos a inserir (n)
 *   - false_positive_rate: taxa de falso positivo desejada (p), ex: 0.01
 *
 * Internamente calcula:
 *   m (tamanho do vetor de bits) = ceil( -(n * ln(p)) / (ln(2))^2 )
 *   k (numero de funções hash)   = round( (m/n) * ln(2) )
 *
 * Retorna um ponteiro para o filtro alocado, ou NULL em caso de erro.
 */
BloomFilter *bloom_create(size_t expected_elements, double false_positive_rate);

/*
 * bloom_destroy
 * -------------
 * Libera toda a memória alocada pelo filtro.
 */
void bloom_destroy(BloomFilter *bf);

/*
 * bloom_insert
 * ------------
 * Insere uma string (item) no filtro, marcando k bits no vetor.
 */
void bloom_insert(BloomFilter *bf, const char *item);

/*
 * bloom_query
 * ------------
 * Consulta se um item pode estar presente no filtro.
 * Retorna:
 *    0 -> DEFINITIVAMENTE NÃO existe (nenhum falso negativo é possível)
 *    1 -> POSSIVELMENTE existe (pode ser falso positivo; checar na Hash)
 */
int bloom_query(const BloomFilter *bf, const char *item);

/*
 * bloom_print_stats
 * ------------------
 * Imprime informações de dimensionamento e ocupação do filtro
 * (tamanho do vetor, k, elementos inseridos, taxa de ocupação estimada).
 */
void bloom_print_stats(const BloomFilter *bf);

/*
 * bloom_estimated_fp_rate
 * ------------------------
 * Calcula a taxa de falso positivo TEÓRICA atual, com base no número
 * de elementos já inseridos (n), no tamanho do vetor (m) e em k:
 *
 *   p_estimada = (1 - e^(-k*n/m))^k
 */
double bloom_estimated_fp_rate(const BloomFilter *bf);

#endif /* BLOOM_H */
