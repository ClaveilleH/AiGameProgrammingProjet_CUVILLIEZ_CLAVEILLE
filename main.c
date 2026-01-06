#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "data.h"
#include "game.h"
#include "bot.h"
#include "logger.h"
#include "weights.h"


Board board;
int run = 1;
int PLAYER = 0; // Joueur courant

int main(int argc, char* argv[]) {
    srand(time(NULL));
    init_logger();
    init_board(&board);
    print_board(&board);

    int autoplay_mode = 0;
    char* weights_bot1 = NULL;
    char* weights_bot2 = NULL;

    // ===== Analyse des arguments =====
    if (argc >= 2 && strcmp(argv[1], "autoplay") == 0) {
        autoplay_mode = 1;
        if (argc >= 3) weights_bot1 = argv[2];
        if (argc >= 4) weights_bot2 = argv[3];
    } else if (argc >= 2) {
        PLAYER = atoi(argv[1]);
        if (PLAYER != 1 && PLAYER != 2) {
            fprintf(stderr, "Invalid player ID. Must be 1 or 2.\n");
            return 1;
        }
        PLAYER -= 1; // Convertir en index 0-based
        if (argc >= 3) weights_bot1 = argv[2];
        load_weights_from_file(weights_bot1);
    } else {
        fprintf(stderr, "Usage: %s <1|2|autoplay> [weights1.cfg] [weights2.cfg]\n", argv[0]);
        return 1;
    }

    // ===== Mode autoplay pour deux bots =====
    if (autoplay_mode) {
        int turn = 0;
        int winner = -1;
        while (run) {
            if (check_end_game(&board, &winner)){
                printf("Player %d wins!\n", winner);
                run = 0;
            }
            else{
                PLAYER = turn;
                if (turn == 0 && weights_bot1){
                printf("bot 1\n");
                load_weights_from_file(weights_bot1);}

                if (turn == 1 && weights_bot2){
                    printf("bot 2\n");
                    load_weights_from_file(weights_bot2);
                } 

                bot_play(&board);
                print_board(&board);

                if (check_winner(&board, &winner)) {
                    printf("Player %d wins!\n", winner);
                    run = 0;
                } else if (check_draw(&board)) {
                    printf("Game ends in a draw!\n");
                    run = 0;
                } else {
                    turn = 1 - turn; // changer de joueur
                }
            } 
            
        }

        printf("FINAL: j1_score=%d j2_score=%d winner=%d\n", board.j1_score, board.j2_score, winner);
        close_logger();
        return 0;
    }

    // ===== Mode joueur humain vs bot =====
    int winner = -1;
    int turn = 0;
    while (run) {
        if (turn == PLAYER) {
            bot_play(&board);
        } else {
            int hole_index;
            SeedType type;
            if (get_human_move(&hole_index, &type)) {
                DEBUG_PRINT("not a valid input\n");
                continue; // Invalid input, ask again
            } else {
                if (!is_valid_move(&board, hole_index, type, 1 - PLAYER)) {
                    fprintf(stderr, "Invalid move. Try again.\n");
                    continue; // Invalid move, ask again
                }
                make_move(&board, hole_index, type, 1 - PLAYER);
                // print_board(&board);
            }
        }
        int end ;
        end = check_end_game(&board, &winner);
        if (end) {
            if (winner == -1) {
                _log("Game ends in a draw!");
                // DEBUG_PRINT("Game ends in a draw!\n");
                fprintf(stderr, "Game ends in a draw!\n");
            } else {
                _log("Player %d wins!", winner + 1);
                // DEBUG_PRINT("Player %d wins!\n", winner + 1);
                fprintf(stderr, "Player %d wins!\n", winner + 1);
            }
            // Lire le coup du joueur humain
            COMPETE_PRINT("Enter your move (hole index and seed type): ");
            if (scanf("%d%s", &hole_index, &type) != 2) {
                fprintf(stderr, "Invalid input\n");
                continue;
            }
            make_move(&board, hole_index-1, type, 1-PLAYER);
        }

        print_board(&board);

        if (check_winner(&board, &winner)) {
            printf("Player %d wins!\n", winner);
            run = 0;
        } else if (check_draw(&board)) {
            printf("Game ends in a draw!\n");
            run = 0;
        } else {
            turn = 1 - turn;
        }
    }

    close_logger();
    return 0;
}
