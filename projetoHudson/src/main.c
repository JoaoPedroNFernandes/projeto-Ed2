#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "hash.h"
#include "bloom_filter.h"

int quantidadeUsuarios = 0;
int consultas = 0;
int consultas2 = 0;
int consultasPositivas = 0;
int consultasPositivas2 = 0;
int consultasNegativas = 0;
int consultasNegativas2 = 0;
int falsoPositivo = 0;
double tempoTotal = 0.0;
double tempoTotal2 = 0.0;

void inserirUsuario(Hash *hash, BloomFilter *bloom, char *usuario) {
    inserirBloom(bloom, usuario);
    inserir(hash, usuario);
    quantidadeUsuarios++;
}

void consultaUsuario(Hash *hash, BloomFilter *bloom, char *usuario) {
    consultas++;
    clock_t tempoInicial = clock();
    int resultadoBloom = buscarBloom(bloom, usuario);
    if(resultadoBloom == 0) {
        printf("Usuario %s, defeinitivamente não existe(Bloom Filter).\n", usuario);
        consultasNegativas++;
    } else {
        printf("Usuario %s possivelmente existe(Bloom Filter).\n", usuario);
        if(buscar(hash, usuario)) {
            printf("Usuario %s, encontrado.\n", usuario);
            consultasPositivas++;
        } else {
            printf("Usuario %s, inexistente. Falso positivo(Bloom Filter)\n", usuario);
            falsoPositivo++;
        }
    }

    clock_t tempoFinal = clock();
    double tempoConsulta = (double)(tempoFinal - tempoInicial) / CLOCKS_PER_SEC;
    tempoTotal += tempoConsulta;    
}

void consultaUsuario2(Hash *hash, BloomFilter *bloom, char *usuario) {
    consultas2++;
    clock_t tempoInicial = clock();

    if(buscar(hash, usuario)) {
        printf("Usuario %s, encontrado.\n", usuario);
        consultasPositivas2++;
    } else {
        printf("Usuario %s, inexistente. Sem Bloom Filter\n", usuario);
        consultasNegativas2++;
    }

    clock_t tempoFinal2 = clock();
    double tempoConsulta2 = (double)(tempoFinal2 - tempoInicial) / CLOCKS_PER_SEC;
    tempoTotal2 += tempoConsulta2;    
}

void mostrarEstatisticas() {
    printf("\nEstatísticas:\n");
    printf("Quantidade de elementos armazenados: %d\n", quantidadeUsuarios);
    printf("Quantidade de consultas realizadas: %d\n", consultas);
    printf("Consultas evitadas pelo Blooom Filter: %d\n", consultasNegativas);
    printf("Consultas positivas (usuário encontrado): %d\n", consultasPositivas);
    printf("Numero de falsos positivos: %d\n", falsoPositivo);
    printf("Tempo médio gasto por consulta: %.6f segundos\n", tempoTotal / consultas);
    printf("Tempo total gasto em consultas: %.6f segundos\n", tempoTotal);
    printf("Tempo médio gasto por consulta sem Bloom Filter: %.6f segundos\n", tempoTotal2 / consultas2);
    printf("Tempo total gasto em consultas sem Bloom Filter: %.6f segundos\n", tempoTotal2);
}

void inserirArquivo(Hash *hash, BloomFilter *bloom, const char *nomeArquivo) {
    //Mudar nome do arquivo para o caminho correto, caso necessário
    FILE *arquivo = fopen("../data/usuarios1000.txt", "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo %s\n", nomeArquivo);
        return;
    }

    char usuario[20];
    while (fgets(usuario, sizeof(usuario), arquivo)) {
        // Remover o caractere de nova linha, se presente
        usuario[strcspn(usuario, "\n")] = '\0';
        inserirUsuario(hash, bloom, usuario);
    }
    printf("Usuários inseridos em lote\n");

    fclose(arquivo);
}

int main() {
    Hash hash;
    inicializarTabelaHash(&hash);

    BloomFilter bloom;
    
    inicializarBloomFilter(&bloom);

    int opcao;

    char usuario[20];

    do {
        printf("\nMenu:\n");
        printf("1. Inserir\n");
        printf("2. Consultar usuário\n");
        printf("3. Inserir usuários em lote\n");
        printf("4. Estatísticas\n");
        printf("5. Sair\n");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1:
                printf("Digite o nome do usuário (até 9 caracteres): ");
                scanf("%s", usuario);
                inserirUsuario(&hash, &bloom, usuario);
                break;
            case 2:
                printf("Digite o nome do usuário para consulta (até 9 caracteres): ");
                scanf("%s", usuario);
                consultaUsuario(&hash, &bloom, usuario);
                consultaUsuario2(&hash, &bloom, usuario);
                break;
            case 3:
                inserirArquivo(&hash, &bloom, "usuarios.txt");
                break;
            case 4:
                mostrarEstatisticas();
                break;
        }
    } while (opcao != 5);

    liberar(&hash);
    return 0;
}