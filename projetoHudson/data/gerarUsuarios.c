#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void gerarUsuario(char *usuario) {
    for(int i=0; i<8; i++) {
        usuario[i] = 'a' + (rand() % 26);
    }
    for(int i=8; i<11; i++) {
        usuario[i] = '0'+(rand() % 10);
    }
    usuario[11] = '\0';
}

void gerarArquivo(char *nomeArquivo, int quantidade) {
    FILE *arquivo = fopen(nomeArquivo, "w");
    if (arquivo == NULL) {
        printf("Erro ao criar o arquivo %s\n", nomeArquivo);
        return;
    }

    char usuario[12];
    for(int i=0; i<quantidade; i++) {
        gerarUsuario(usuario);
        fprintf(arquivo, "%s\n", usuario);
    }

    fclose(arquivo);
}

int main() {
    gerarArquivo("usuarios1000.txt", 1000);
    gerarArquivo("usuarios10000.txt", 10000);
    gerarArquivo("usuarios100000.txt", 100000);
    return 0;
}