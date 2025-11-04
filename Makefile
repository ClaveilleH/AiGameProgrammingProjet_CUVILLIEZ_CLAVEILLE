# Makefile simple pour AIGame
CC = gcc
# CC = clang

# Mode de compilation (release par défaut, debug optionnel)
MODE ?= release

# Flags selon le mode
ifeq ($(MODE),debug)
	CFLAGS = -Wall -g -O3 -march=native -DDEBUG=1
else ifeq ($(MODE),compete)
	# Mode 'compete' : désactive les logs debug et active la macro GAME
	CFLAGS = -Wall -g -O3 -march=native -DCOMPETE=1
else
	CFLAGS = -Wall -g -O3 -march=native
endif

TARGET = aigame
SOURCES = main.c data.c game.c bot.c logger.c

# Compilation
all: $(TARGET)

$(TARGET): $(SOURCES)
	@echo "Compiling in $(MODE) mode..."
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES)

# Nettoyage
clean:
	rm -f $(TARGET) *.o

# Exécution
run: $(TARGET)
	./$(TARGET)

# Reconstruction
rebuild: clean all

.PHONY: all clean run rebuild


# make MODE=debug rebuild run 
# make MODE=compete