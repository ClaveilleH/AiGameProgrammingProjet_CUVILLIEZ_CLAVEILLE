#ifndef DATA_H
#define DATA_H

#include <stdio.h>
#include <stdlib.h>

/*
Idée : on cree une structure et les fonctions pour manipuler cette structure
comme ca si on change de structure on change juste le code dans data.h et data.c
et pas dans tout le code
*/


#define MAX_HOLES 16
#define MAX_SEEDS 96
#define PLAYERS 1

typedef enum {
    R,
    B,
    TR,
    TB
} SeedType;

typedef struct {
    int R;
    int B;
    int T;
} Hole;

typedef struct {
    Hole holes[16];
    int seed_count;
    int j1_score;
    int j2_score;
} Board;

void init_board(Board* board);

Hole* get_hole(Board* board, int index);

void print_board(const Board* board);


// Board* create_game();
// Board* copy_game(const Board* board);
// Hole* get_hole(Board* board, int index);
// int get_color_count(const Hole* hole, int color);

#endif // DATA_H