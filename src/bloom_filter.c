#include <stdio.h>
#include <string.h>

#include "bloom_filter.h"

void inicializarBloomFilter(BloomFilter *bloom) {
    for (int i = 0; i < TAM_BLOOM; i++) {
        bloom->bits[i] = 0;
    }

    bloom->quantidadeHashe = 3;
}

int hashBloom1(char *chave) {
    int soma = 0;

    for (int i = 0; chave[i] != '\0'; i++) {
        soma += chave[i];
    }

    return soma % TAM_BLOOM;
}

int hashBloom2(char *chave) {
    int hash = 5381;

    for (int i = 0; chave[i] != '\0'; i++) {
        hash = ((hash << 5) + hash) + chave[i];
    }

    return hash % TAM_BLOOM;
}

int hashBloom3(char *chave) {
    int hash = 0;

    for (int i = 0; chave[i] != '\0'; i++) {
        hash = (hash * 31) + chave[i];
    }

    return hash % TAM_BLOOM;
}

void inserirBloom(BloomFilter *bloom, char *usuario) {
    bloom->bits[hashBloom1(usuario)] = 1;
    bloom->bits[hashBloom2(usuario)] = 1;
    bloom->bits[hashBloom3(usuario)] = 1;
}

int buscarBloom(BloomFilter *bloom, char *usuario) {
    if (bloom->bits[hashBloom1(usuario)] &&
        bloom->bits[hashBloom2(usuario)] &&
        bloom->bits[hashBloom3(usuario)]) {
        return 1;
    }
    
    return 0;
}