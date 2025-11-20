# Makefile simple pour AIGame
CC = gcc
# CC = clang

# Mode de compilation (release par défaut, debug optionnel)
MODE ?= release
TEST_SOURCES = data.c game.c bot.c logger.c evaluate.c

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
SOURCES = main.c data.c game.c bot.c logger.c evaluate.c

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

# Cible pour les tests
test: tests.c $(TEST_SOURCES)
	@echo "Compiling tests..."
	$(CC) $(CFLAGS) -march=native -DCOMPETE=1 -o test_aigame tests.c $(TEST_SOURCES)

dtest: tests.c $(TEST_SOURCES)
	@echo "Compiling tests..."
	$(CC) $(CFLAGS) -DDEBUG=1 -o test_aigame tests.c $(TEST_SOURCES)

# Exécuter les tests
run-test: test
	./test_aigame


.PHONY: all clean run rebuild test dtest run-test


# make MODE=debug rebuild run 
# make MODE=compete