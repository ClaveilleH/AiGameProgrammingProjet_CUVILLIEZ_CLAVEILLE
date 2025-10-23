#include <stdio.h>
#include <stdlib.h>

#include "data.h"
#include "game.h"





Board board;


int main(int argc, char* argv[]) {
    int run = 1;
    init_board(&board);
    print_board(&board);
    while (run) {
        // Game loop
        int winner = 0;
        if (check_winner(&board, &winner)) {
            printf("Player %d wins!\n", winner);
            run = 0;
        } else {
            // Continue game
            printf("Game continues...\n");
        }

        // make_move(&board, 0, R);
        // print_board(&board);


        // make_move(&board, 0, B);
        // print_board(&board);

        make_move(&board, 1, TB);
        print_board(&board);

        break; // Placeholder to avoid infinite loop in this example
    }

    return 0;
}