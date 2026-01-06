# Makefile para Jogo com Allegro 5

# 1. Variáveis de Compilação
# -------------------------
# O compilador que será usado
CC = gcc

# O nome do arquivo executável final
EXEC = jogo

# Lista de todos os seus arquivos de código-fonte (.c)
SRCS = jogo.c Entity.c Joystick.c Bullet.c Pistol.c

# Gera automaticamente a lista de arquivos objeto (.o)
OBJS = $(SRCS:.c=.o)

# MELHORIA: Lista de todos os módulos do Allegro que você usa
PKG_MODULES = allegro-5 allegro_main-5 allegro_audio-5 allegro_acodec-5 allegro_font-5 allegro_ttf-5 allegro_image-5 allegro_primitives-5

# Flags do compilador, geradas a partir da lista de módulos
CFLAGS = $(shell pkg-config $(PKG_MODULES) --cflags)

# Flags do linker, geradas a partir da MESMA lista de módulos
# CORREÇÃO: Removido o 'allegro_ADDON-5' e adicionado o resto.
LIBS = $(shell pkg-config $(PKG_MODULES) --libs)


# 2. Regras de Compilação
# ---------------------
# A regra "all" é a padrão.
.PHONY: all
all: $(EXEC)

# Regra para criar o executável final
$(EXEC): $(OBJS)
	@echo "Ligando os objetos para criar o executável..."
	$(CC) $(OBJS) -o $(EXEC) $(LIBS)
	@echo "Programa '$(EXEC)' compilado com sucesso!"

# Regra genérica para criar arquivos .o a partir de .c
%.o: %.c
	@echo "Compilando $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Regra "clean" para limpar os arquivos gerados
.PHONY: clean
clean:
	@echo "Limpando arquivos gerados..."
	rm -f $(EXEC) $(OBJS)

# Regra "re" para forçar uma recompilação completa
.PHONY: re
re: clean all
