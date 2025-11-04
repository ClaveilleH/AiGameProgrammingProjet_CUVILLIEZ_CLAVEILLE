#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>

#include "data.h"
#include "game.h"
#include "bot.h"
#include "logger.h"
#include <time.h>


Board board;

int get_human_move(int *hole_index, SeedType *type) {
    COMPETE_PRINT("Enter your move (hole index and seed type): ");
    int idx;
    char type_str[10]; // Buffer pour lire la chaîne complète

    if (scanf("%d%s", &idx, type_str) != 2) {
        fprintf(stderr, "Invalid input format. got %d %s\n", idx, type_str);
        // Nettoyer le buffer d'entrée
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        return 1;
    }

    if (idx <= 0 || idx > MAX_HOLES) {
        fprintf(stderr, "Invalid hole index. Must be between 0 and %d.\n", MAX_HOLES - 1);
        return 1;
    }

    SeedType seed_type;
    if (strcmp(type_str, "R") == 0) {
        seed_type = R;
    } else if (strcmp(type_str, "B") == 0) {
        seed_type = B;
    } else if (strcmp(type_str, "TR") == 0) {
        seed_type = TR;
    } else if (strcmp(type_str, "TB") == 0) {
        seed_type = TB;
    } else {
        fprintf(stderr, "Invalid seed type. Use 'R', 'B', 'TR', or 'TB' entered %s.\n", type_str);
        return 1;
    }
    log("Other player chose hole %d with seed type %s", idx,
        (seed_type == R) ? "R" : 
        (seed_type == B) ? "B" : 
        (seed_type == TR) ? "TR" : "TB");

    *hole_index = idx - 1; // Convertir en index 0-based
    *type = seed_type;
    return 0;
}



int main(int argc, char* argv[]) {
    int run = 1;
    int turn = 0; // 0 for player 1, 1 for player 2
    int winner = -1;
    srand(time(NULL));

    init_logger();

    // le player id est passé en argument
    if (argc >= 2) {
        PLAYER = atoi(argv[1]);
    } else {
        // Sinon on demande a l'utilisateur
        COMPETE_PRINT("Enter bot ID (1 or 2): ");
        if (scanf("%d", &PLAYER) != 1 || (PLAYER != 1 && PLAYER != 2)) {
            fprintf(stderr, "Invalid player ID. Must be 1 or 2.\n");
            return 1;
        }
        PLAYER -= 1; // Convert to 0-based index
    }
    fprintf(stderr, "Starting game as Player %d\n", PLAYER + 1);
    log("Game started as Player %d", PLAYER + 1);
    init_board(&board);
    print_board(&board);
    while (run) {
        // Game loop
        if (turn == PLAYER) {
            DEBUG_PRINT("Bot's turn.\n");
            bot_play(&board);
        } else {
            DEBUG_PRINT("Player's turn.\n");
            int hole_index;
            SeedType type;
            if (get_human_move(&hole_index, &type)) {
                continue; // Invalid input, ask again
            } else {
                if (!is_valid_move(&board, hole_index, type, 1 - PLAYER)) {
                    fprintf(stderr, "Invalid move. Try again.\n");
                    continue; // Invalid move, ask again
                }
                make_move(&board, hole_index, type);
                print_board(&board);
            }
        }

        if (check_winner(&board, &winner)) {
            DEBUG_PRINT("Player %d wins!\n", winner);
            run = 0;
        } else {
            // Continue game
            DEBUG_PRINT("Game continues...\n");
        }
        print_board(&board);
        turn = 1 - turn;
        // break; // Placeholder to avoid infinite loop in this example
    }

    return 0;
}



/*

make -mode

*/