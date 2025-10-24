# Makefile simple pour AIGame
CC = gcc
# CC = clang
CFLAGS = -Wall -g -o3 -march=native
# CFLAGS = -Wall -g --- IGNORE ---
TARGET = aigame
SOURCES = main.c data.c game.c bot.c

# Compilation
all: $(TARGET)

$(TARGET): $(SOURCES)
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