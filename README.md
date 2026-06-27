# Sistema de Consulta Eficiente com Tabela Hash e Filtro de Bloom

## Instruções de compilação

```bash
# Compilar o sistema principal
make

# Gerar arquivos de dados de teste (1k, 10k, 100k usuários)
make gerar

# Limpar binários e dados gerados
make clean
```

Requisito: `gcc` instalado.

## Estrutura do projeto

```
projeto/
├── src/
│   ├── hash.c / hash.h          # Tabela hash com encadeamento externo
│   ├── bloom_filter.c / bloom_filter.h  # Filtro de Bloom (vetor de bits)
│   └── main.c                   # Integração, menu e experimentos
├── data/
│   ├── gerarUsuarios.c          # Gerador de nomes aleatórios
│   └── usuarios*.txt            # Gerados por `make gerar`
├── testes/
├── Makefile
└── README.md
```

## Formato de entrada

Nomes de usuário seguem o padrão `[8 letras][3 dígitos]`, por exemplo:

```
islaifda122
djskalsa297
fjkldsaf881
```

Arquivos de lote: um nome por linha, sem cabeçalho.

## Exemplos de execução

```bash
# Iniciar o sistema
./src/main
```

### Menu interativo

```
1. Inserir usuario
2. Consultar (com Bloom Filter)
3. Consultar (sem Bloom Filter)
4. Inserir em lote (arquivo)
5. Estatisticas
6. Executar experimento
0. Sair
```

### Inserção em lote e experimento

```
Opcao: 4
Caminho do arquivo: data/usuarios1000.txt
Carregados 1000 usuarios de 'data/usuarios1000.txt'.

Opcao: 6
Caminho do arquivo para experimento: data/usuarios100000.txt
--- Experimento: 'data/usuarios100000.txt' ---
Elementos carregados: 100000

Elementos     T. sem Bloom (s)  T. com Bloom (s)  Falsos Positivos
100000        0.057630          0.097960          3.31%
```

### Consulta individual

```
Opcao: 2
Nome do usuario: joao123
Bloom: definitivamente nao existe. Consulta a hash evitada.

Opcao: 2
Nome do usuario: islaifda122
Usuario 'islaifda122' encontrado.
```
