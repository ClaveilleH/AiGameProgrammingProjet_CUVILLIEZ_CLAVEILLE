#include <stdio.h>
#include <stdlib.h>
#include "data.h"

void init_board(Board* board) {
    for (int i = 0; i < 16; i++) {
        board->holes[i].R = 2;
        board->holes[i].B = 2;
        board->holes[i].T = 2;
    }
    board->seed_count = 96;
}


void print_board(const Board* board) {
    printf("Board state:\n");
    for (int i = 0; i < 16; i++) {
        printf("Hole ");
        if (i < 10) {
            printf(" ");
        }
        printf("%d: R=", i);
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

