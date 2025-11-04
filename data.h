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
#define MASK (MAX_HOLES - 1)
#define MAX_SEEDS 96
// #define PLAYER 1

#ifdef DEBUG
    #define DEBUG_PRINT(fmt, args...) fprintf(stderr, "DEBUG: " fmt, ## args)
#else
    #define DEBUG 0
    #define DEBUG_PRINT(fmt, args...) /* Don't do anything in release builds */
#endif

#ifndef COMPETE
    #define COMPETE 0
    #define COMPETE_PRINT(fmt, args...) fprintf(stderr, fmt, ## args)
#else
    #define COMPETE_PRINT(fmt, args...) /* Don't do anything in compete builds */
#endif

extern int PLAYER; // global variable to hold player id

typedef enum {
    R,
    B,
    TR,
    TB
} SeedType;

typedef struct {
    __uint8_t R;
    __uint8_t B;
    __uint8_t T;
    __uint8_t __pad;
} Hole;

typedef struct {
    Hole holes[MAX_HOLES    ];
    int seed_count;
    int j1_score;
    int j2_score;
} Board;

void init_board(Board* board);


void print_board(const Board* board);

static inline Hole* get_hole(Board* board, int index) {
    if (index < 0 || index >= 16) {
        fprintf(stderr, "Invalid hole index\n");
        exit(EXIT_FAILURE);
    }
    return &board->holes[index];
}

// static inline Hole* get_all_holes(Board* board) {
//     return board->holes;
// }

// static inline int get_seed_count(const Board* board) {
//     return board->seed_count;
// }




#endif // DATA_H