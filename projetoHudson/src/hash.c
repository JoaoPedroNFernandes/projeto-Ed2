#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash.h"

void inicializarTabelaHash(Hash *hash) {
    for (int i = 0; i < TAM_HASH; i++) {
        hash->tabela[i] = NULL;
    }
}

int funcaoHash(char *chave) {
    int soma = 0;

    for (int i = 0; chave[i] != '\0'; i++) {
        soma += chave[i];
    }

    return soma % TAM_HASH;
}

void inserir(Hash *hash, char *nome) {
    int indice = funcaoHash(nome);
    Usuario *novo = malloc(sizeof(Usuario));
    strcpy(novo->nome, nome);
    novo->prox = hash->tabela[indice];
    hash->tabela[indice] = novo;
}

int buscar(Hash *hash, char *nome) {
    int indice = funcaoHash(nome);
    Usuario *atual = hash->tabela[indice];

    while (atual != NULL) {
        if (strcmp(atual->nome, nome) == 0) {
            return 1;
        }
        atual = atual->prox;
    }

    return 0;
}

void liberar(Hash *hash) {
    for (int i = 0; i < TAM_HASH; i++) {
        Usuario *atual = hash->tabela[i];
        while (atual != NULL) {
            Usuario *temp = atual;
            atual = atual->prox;
            free(temp);
        }
    }
}