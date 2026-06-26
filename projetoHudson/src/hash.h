#ifndef HASH_H
#define HASH_H

#define TAM_HASH 1000

typedef struct Usuario{
    char nome[20];
    struct Usuario *prox;
} Usuario;

typedef struct {
    Usuario *tabela[TAM_HASH];
} Hash;

void inicializarTabelaHash(Hash *hash);
int funcaoHash(char *chave);

void inserir(Hash *hash, char *nome);
int buscar(Hash *hash, char *nome);
void liberar(Hash *hash);

#endif