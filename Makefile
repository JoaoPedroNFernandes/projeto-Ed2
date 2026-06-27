CC      = gcc
CFLAGS  = -Wall -Wextra -Wno-unused-result -O2
SRCDIR  = src
DATADIR = data
TARGET  = $(SRCDIR)/main

SRCS = $(SRCDIR)/main.c $(SRCDIR)/hash.c $(SRCDIR)/bloom.c

.PHONY: all clean gerar

all: $(TARGET)

$(TARGET): $(SRCS) $(SRCDIR)/hash.h $(SRCDIR)/bloom.h
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS) -lm

# Compila e executa o gerador de dados de teste
gerar:
	$(CC) $(CFLAGS) -o $(DATADIR)/gerar $(DATADIR)/gerarUsuarios.c
	cd $(DATADIR) && ./gerar

clean:
	rm -f $(TARGET) $(DATADIR)/gerar \
	      $(DATADIR)/usuarios1000.txt \
	      $(DATADIR)/usuarios10000.txt \
	      $(DATADIR)/usuarios100000.txt
