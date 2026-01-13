#ifndef DATA_H
#define DATA_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#ifdef _WIN32
#define UINT8 uint8_t
#else
#define UINT8 __uint8_t
#endif
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
    UINT8 R;
    UINT8 B;
    UINT8 T;
    UINT8 __pad;
} Hole;

typedef struct {
    Hole holes[MAX_HOLES]; 
    int seed_count;     // total number of seeds on the board
    int j1_score;       // score of player 1
    int j2_score;       // score of player 2
    int nb_coups_player1; // number of moves made by player 1
    int nb_coups_player2; // number of moves made by player 2 (+- 1)
} Board;

typedef struct {
    UINT8 hole_index;
    SeedType type;
} Move;

// Constante pour représenter un coup invalide
#define INVALID_MOVE ((Move){.hole_index = 255, .type = R})

// Fonction inline pour vérifier si un Move est valide
static inline int is_move_valid(Move move) {
    return move.hole_index != 255;
}

// Fonction inline pour créer un Move invalide
static inline Move make_invalid_move(void) {
    return INVALID_MOVE;
}

typedef struct MoveList {
    Move* moves;
    struct MoveList* next;
} MoveList;

void init_board(Board* board);
void print_board(const Board* board);
void force_print_board(const Board* board);

static inline Hole* get_hole(Board* board, int index) {
    if (index < 0 || index >= 16) {
        fprintf(stderr, "Invalid hole index : %d\n", index);
        exit(EXIT_FAILURE);
    }
    return &board->holes[index];
}


Board* dup_board(const Board* board);
int free_board(Board* board);


int get_total_seeds(Hole* hole);
int get_score(Board* board, int playerId);
int get_nb_coups(Board* board, int playerId);

#endif // DATA_H