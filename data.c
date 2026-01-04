#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data.h"

int PLAYER;

void init_board(Board* board) {
    for (int i = 0; i < 16; i++) {
        board->holes[i].R = 2;
        board->holes[i].B = 2;
        board->holes[i].T = 2;
    }
    board->seed_count = 96;
    board->j1_score = 0;
    board->j2_score = 0;
    
}

Board* dup_board(const Board* board) {
    /*
    Fait une copie profonde du plateau de jeu.
    */
    Board* new_board = (Board*)malloc(sizeof(Board));
    if (!new_board) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    memcpy(new_board, board, sizeof(Board));
    return new_board;
}

int free_board(Board* board) {
    /*
    Libère la mémoire allouée pour le plateau de jeu.
    */
    if (board) {
        free(board);
        return 0; // Succès
    }
    return -1; // Échec
}


int get_total_seeds(Hole* hole) {
    DEBUG_PRINT("Calculating total seeds: R=%d, B=%d, T=%d\n", hole->R, hole->B, hole->T);
    return (hole->R) + (hole->B) + (hole->T);
}

int get_score(Board* board, int playerId) {
    if (playerId == 0) {
        return board->j1_score;
    } else if (playerId == 1) {
        return board->j2_score;
    } else {
        fprintf(stderr, "Invalid player ID: %d\n", playerId);
        return -1; // Indicate an error
    }
}

void print_board(const Board* board) {
    if (!DEBUG) return;
    printf("Score - Player 1: %d, Player 2: %d\n", board->j1_score, board->j2_score);
    printf("Board state:\n");
    for (int i = 0; i < 16; i++) {
        printf("Hole ");
        if (i < 10) {
            printf(" ");
        }
        printf("%d (%d): R=", i, i+1);
        if (board->holes[i].R < 10) {
            printf(" ");
        }
        printf("%d, B=", board->holes[i].R);
        if (board->holes[i].B < 10) {
            printf(" ");
        }
        printf("%d, T=", board->holes[i].B);
        if (board->holes[i].T < 10) {
            printf(" ");
        }
        printf("%d\n", board->holes[i].T);
        // printf("Hole %d: R=%d, B=%d, T=%d\n", i, board->holes[i].R, board->holes[i].B, board->holes[i].T);
    }
    printf("Total seed count: %d\n", board->seed_count);
}

