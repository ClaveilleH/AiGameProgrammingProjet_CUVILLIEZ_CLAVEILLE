# Makefile simple pour AIGame
# Compilateur par défaut (Linux)
CC = gcc
# CC = clang

# Pour compilation Windows depuis Linux, décommentez :
# CC = x86_64-w64-mingw32-gcc

# Mode de compilation (release par défaut, debug optionnel)
MODE ?= release
TEST_SOURCES = data.c game.c bot.c logger.c evaluate.c

# Flags selon le mode
ifeq ($(MODE),debug)
    CFLAGS = -Wall -g -O3 -march=native -DDEBUG=1
else ifeq ($(MODE),compete)
    # Mode 'compete' : désactive les logs debug et active la macro GAME
    CFLAGS = -Wall -g -O3 -march=native -DCOMPETE=1
else ifeq ($(MODE),win)
    # Mode 'win' : compilation pour Windows
    CC = x86_64-w64-mingw32-gcc
    CFLAGS = -Wall -g -O3 -DCOMPETE=1
else
    CFLAGS = -Wall -g -O3 -march=native
endif

# Extension selon la plateforme
ifeq ($(CC),x86_64-w64-mingw32-gcc)
    EXT = .exe
    # Retirer -march=native pour Windows
    CFLAGS := $(filter-out -march=native,$(CFLAGS))
else ifeq ($(CC),i686-w64-mingw32-gcc)
    EXT = .exe
    CFLAGS := $(filter-out -march=native,$(CFLAGS))
else
    EXT =
endif

TARGET = aigame$(EXT)
SOURCES = main.c data.c game.c bot.c logger.c evaluate.c

# Compilation
all: $(TARGET)

$(TARGET): $(SOURCES)
	@echo "Compiling in $(MODE) mode with $(CC)..."
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES) -lm

# Nettoyage
clean:
	rm -f aigame aigame.exe try_aigame try_aigame.exe test_aigame test_aigame.exe *.o

# Exécution
run: $(TARGET)
	./$(TARGET)

# Reconstruction
rebuild: clean all

# Cible pour try (compile try.c comme main.c)
try: try.c data.c game.c bot.c logger.c evaluate.c arbre.c
	@echo "Compiling try in $(MODE) mode..."
	$(CC) $(CFLAGS) -o try_aigame$(EXT) try.c data.c game.c bot.c logger.c evaluate.c arbre.c genererDot.c -lm
	./try_aigame$(EXT)
	dot -Tpng graphe.dot -o graphe2.png

# Cible pour les tests
test: tests.c $(TEST_SOURCES)
	@echo "Compiling tests..."
	$(CC) $(CFLAGS) -DCOMPETE=1 -o test_aigame$(EXT) tests.c $(TEST_SOURCES) -lm

dtest: tests.c $(TEST_SOURCES)
	@echo "Compiling tests..."
	$(CC) $(CFLAGS) -DDEBUG=1 -o test_aigame$(EXT) tests.c $(TEST_SOURCES) -lm

# Exécuter les tests
run-test: test
	./test_aigame$(EXT)

# Cible pour compilation Windows
windows:
	$(MAKE) CC=x86_64-w64-mingw32-gcc

windows32:
	$(MAKE) CC=i686-w64-mingw32-gcc

.PHONY: all clean run rebuild try test dtest run-test windows windows32
