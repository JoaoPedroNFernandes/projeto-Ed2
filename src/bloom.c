#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "bloom.h"

static unsigned long hash_djb2(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = (unsigned char)*str++))
        hash = ((hash << 5) + hash) + c;
    return hash;
}

static unsigned long hash_fnv1a(const char *str) {
    unsigned long hash = 2166136261UL;
    int c;
    while ((c = (unsigned char)*str++)) {
        hash ^= (unsigned long)c;
        hash *= 16777619UL;
    }
    return hash;
}

static void set_bit(unsigned char *bits, size_t pos) {
    bits[pos / 8] |= (unsigned char)(1u << (pos % 8));
}

static int get_bit(const unsigned char *bits, size_t pos) {
    return (bits[pos / 8] >> (pos % 8)) & 1u;
}

BloomFilter *bloom_create(size_t expected_elements, double false_positive_rate) {
    if (expected_elements == 0)
        expected_elements = 1;
    if (false_positive_rate <= 0.0 || false_positive_rate >= 1.0) {
        fprintf(stderr, "Taxa de falso positivo invalida (deve estar entre 0 e 1).\n");
        return NULL;
    }

    BloomFilter *bf = (BloomFilter *)malloc(sizeof(BloomFilter));
    if (!bf) return NULL;

    double n = (double)expected_elements;
    double p = false_positive_rate;

    double m_real = -(n * log(p)) / (log(2.0) * log(2.0));
    size_t m = (size_t)ceil(m_real);
    if (m < 8) m = 8;

    double k_real = ((double)m / n) * log(2.0);
    int k = (int)round(k_real);
    if (k < 1) k = 1;

    bf->m = m;
    bf->k = k;
    bf->n = 0;
    bf->expected_n = expected_elements;
    bf->target_fp_rate = p;

    size_t num_bytes = (m + 7) / 8;
    bf->bits = (unsigned char *)calloc(num_bytes, sizeof(unsigned char));
    if (!bf->bits) {
        free(bf);
        return NULL;
    }

    return bf;
}

void bloom_destroy(BloomFilter *bf) {
    if (!bf) return;
    free(bf->bits);
    free(bf);
}

void bloom_insert(BloomFilter *bf, const char *item) {
    if (!bf || !item) return;

    unsigned long h1 = hash_djb2(item);
    unsigned long h2 = hash_fnv1a(item);

    for (int i = 0; i < bf->k; i++) {
        size_t pos = (size_t)((h1 + (unsigned long)i * h2) % bf->m);
        set_bit(bf->bits, pos);
    }

    bf->n++;
}

int bloom_query(const BloomFilter *bf, const char *item) {
    if (!bf || !item) return 0;

    unsigned long h1 = hash_djb2(item);
    unsigned long h2 = hash_fnv1a(item);

    for (int i = 0; i < bf->k; i++) {
        size_t pos = (size_t)((h1 + (unsigned long)i * h2) % bf->m);
        if (!get_bit(bf->bits, pos))
            return 0;
    }

    return 1;
}

double bloom_estimated_fp_rate(const BloomFilter *bf) {
    if (!bf || bf->m == 0) return 0.0;
    double exponent = -((double)bf->k * (double)bf->n) / (double)bf->m;
    double base = 1.0 - exp(exponent);
    return pow(base, bf->k);
}

void bloom_print_stats(const BloomFilter *bf) {
    if (!bf) return;
    printf("---- Estatisticas do Filtro de Bloom ----\n");
    printf("Tamanho do vetor de bits (m): %zu bits (%.2f KB)\n",
           bf->m, (double)(bf->m + 7) / 8.0 / 1024.0);
    printf("Quantidade de funcoes hash (k): %d\n", bf->k);
    printf("Elementos esperados (dimensionamento): %zu\n", bf->expected_n);
    printf("Elementos efetivamente inseridos (n):  %zu\n", bf->n);
    printf("Taxa de falso positivo desejada (p):   %.4f%%\n", bf->target_fp_rate * 100.0);
    printf("Taxa de falso positivo estimada agora: %.4f%%\n", bloom_estimated_fp_rate(bf) * 100.0);
    printf("-------------------------------------------\n");
}
