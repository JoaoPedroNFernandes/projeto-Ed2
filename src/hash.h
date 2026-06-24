#ifndef HASH_H
#define HASH_H

typedef struct NoHash {
    char *chave;
    struct NoHash *proximo;
} NoHash;

typedef struct {
    NoHash **buckets;
    int tamanho;
    int num_elementos;
    unsigned long colisoes;
} TabelaHash;

typedef struct {
    int tamanho_tabela;
    int num_elementos;
    unsigned long colisoes;
    double fator_carga;
    int bucket_mais_longo;
    int buckets_vazios;
    double media_elementos_por_bucket_ocupado;
} EstatisticasHash;

TabelaHash *hash_criar(int tamanho);

void hash_destruir(TabelaHash *tabela);

unsigned long hash_function(const char *chave, int tamanho_tabela);

int hash_inserir(TabelaHash *tabela, const char *chave);

int hash_buscar(TabelaHash *tabela, const char *chave);

double hash_fator_carga(const TabelaHash *tabela);

EstatisticasHash hash_obter_estatisticas(const TabelaHash *tabela);

void hash_imprimir_estatisticas(const TabelaHash *tabela);

#endif