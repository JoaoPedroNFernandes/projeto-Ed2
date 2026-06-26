#ifndef BLOOM_FILTER_H
#define BLOOM_FILTER_H

#define TAM_BLOOM 100000

typedef struct {
    unsigned char bits[TAM_BLOOM];
    int quantidadeHashe;
} BloomFilter;

void inicializarBloomFilter(BloomFilter *bloom);
int hashBloom1(char *chave);
int hashBloom2(char *chave);
int hashBloom3(char *chave);

void inserirBloom(BloomFilter *bloom, char *chave);
int buscarBloom(BloomFilter *bloom, char *chave);

#endif
