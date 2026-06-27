#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash.h"

static char *duplicar_string(const char *origem) {
    if (origem == NULL)
        return NULL;
    size_t tamanho = strlen(origem) + 1;
    char *copia = (char *)malloc(tamanho);
    if (copia != NULL)
        memcpy(copia, origem, tamanho);
    return copia;
}

unsigned long hash_function(const char *chave, int tamanho_tabela) {
    unsigned long hash = 5381UL;
    int c;

    if (chave == NULL || tamanho_tabela <= 0)
        return 0UL;

    while ((c = (unsigned char)(*chave)) != '\0') {
        hash = ((hash << 5) + hash) + (unsigned long)c;
        chave++;
    }

    return hash % (unsigned long)tamanho_tabela;
}

TabelaHash *hash_criar(int tamanho) {
    if (tamanho <= 0)
        return NULL;

    TabelaHash *tabela = (TabelaHash *)malloc(sizeof(TabelaHash));
    if (tabela == NULL)
        return NULL;

    tabela->buckets = (NoHash **)calloc((size_t)tamanho, sizeof(NoHash *));
    if (tabela->buckets == NULL) {
        free(tabela);
        return NULL;
    }

    tabela->tamanho = tamanho;
    tabela->num_elementos = 0;
    tabela->colisoes = 0UL;

    return tabela;
}

void hash_destruir(TabelaHash *tabela) {
    if (tabela == NULL)
        return;

    for (int i = 0; i < tabela->tamanho; i++) {
        NoHash *atual = tabela->buckets[i];
        while (atual != NULL) {
            NoHash *proximo = atual->proximo;
            free(atual->chave);
            free(atual);
            atual = proximo;
        }
    }

    free(tabela->buckets);
    free(tabela);
}

int hash_inserir(TabelaHash *tabela, const char *chave) {
    if (tabela == NULL || chave == NULL)
        return 0;

    unsigned long indice = hash_function(chave, tabela->tamanho);

    NoHash *atual = tabela->buckets[indice];
    while (atual != NULL) {
        if (strcmp(atual->chave, chave) == 0)
            return 0;
        atual = atual->proximo;
    }

    if (tabela->buckets[indice] != NULL)
        tabela->colisoes++;

    NoHash *novo = (NoHash *)malloc(sizeof(NoHash));
    if (novo == NULL)
        return 0;

    novo->chave = duplicar_string(chave);
    if (novo->chave == NULL) {
        free(novo);
        return 0;
    }

    novo->proximo = tabela->buckets[indice];
    tabela->buckets[indice] = novo;
    tabela->num_elementos++;
    return 1;
}

int hash_buscar(TabelaHash *tabela, const char *chave) {
    if (tabela == NULL || chave == NULL)
        return 0;

    unsigned long indice = hash_function(chave, tabela->tamanho);

    NoHash *atual = tabela->buckets[indice];
    while (atual != NULL) {
        if (strcmp(atual->chave, chave) == 0)
            return 1;
        atual = atual->proximo;
    }

    return 0;
}

double hash_fator_carga(const TabelaHash *tabela) {
    if (tabela == NULL || tabela->tamanho == 0)
        return 0.0;
    return (double)tabela->num_elementos / (double)tabela->tamanho;
}

EstatisticasHash hash_obter_estatisticas(const TabelaHash *tabela) {
    EstatisticasHash stats;
    memset(&stats, 0, sizeof(EstatisticasHash));

    if (tabela == NULL)
        return stats;

    stats.tamanho_tabela = tabela->tamanho;
    stats.num_elementos = tabela->num_elementos;
    stats.colisoes = tabela->colisoes;
    stats.fator_carga = hash_fator_carga(tabela);

    int bucket_mais_longo = 0;
    int buckets_vazios = 0;
    int buckets_ocupados = 0;

    for (int i = 0; i < tabela->tamanho; i++) {
        int comprimento = 0;
        NoHash *atual = tabela->buckets[i];

        while (atual != NULL) {
            comprimento++;
            atual = atual->proximo;
        }

        if (comprimento == 0)
            buckets_vazios++;
        else
            buckets_ocupados++;

        if (comprimento > bucket_mais_longo)
            bucket_mais_longo = comprimento;
    }

    stats.bucket_mais_longo = bucket_mais_longo;
    stats.buckets_vazios = buckets_vazios;
    stats.media_elementos_por_bucket_ocupado =
        (buckets_ocupados > 0)
            ? (double)tabela->num_elementos / (double)buckets_ocupados
            : 0.0;

    return stats;
}

void hash_imprimir_estatisticas(const TabelaHash *tabela) {
    if (tabela == NULL) {
        printf("Tabela hash invalida (NULL).\n");
        return;
    }

    EstatisticasHash stats = hash_obter_estatisticas(tabela);

    printf("---------- Estatisticas da Tabela Hash ----------\n");
    printf("Tamanho da tabela..............: %d\n", stats.tamanho_tabela);
    printf("Elementos armazenados..........: %d\n", stats.num_elementos);
    printf("Colisoes totais.................: %lu\n", stats.colisoes);
    printf("Fator de carga...................: %.4f\n", stats.fator_carga);
    printf("Maior bucket (pior caso busca)...: %d elemento(s)\n", stats.bucket_mais_longo);
    printf("Buckets vazios....................: %d (%.1f%%)\n",
           stats.buckets_vazios,
           stats.tamanho_tabela > 0
               ? (100.0 * stats.buckets_vazios / stats.tamanho_tabela)
               : 0.0);
    printf("Media por bucket ocupado..........: %.2f\n",
           stats.media_elementos_por_bucket_ocupado);
    printf("---------------------------------------------------\n");
}
