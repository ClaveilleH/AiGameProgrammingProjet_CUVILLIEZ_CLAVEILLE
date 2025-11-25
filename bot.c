#include "data.h"
#include "game.h"
#include "bot.h"
#include "logger.h"
#include "evaluate.h"


#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

int get_move_list(Board* board, Move* move_list, int player) { 
 
    int index = 0;
    // DEBUG_PRINT("Possible moves : \n"); 
    // Function to get possible moves
    for (int i = player ; i < MAX_HOLES; i+=2) { 
        // Check each hole for possible moves 
        Hole* hole = get_hole(board, i); 
        int nb_graines_R = hole->R; 
        int nb_graines_B = hole->B; 
        int nb_graines_T = hole->T; 
        if(nb_graines_B>0) {
            move_list[index++] = (Move){i, B};
            // DEBUG_PRINT("Move B, hole %d\n", i ); 
        }
        if(nb_graines_R>0) {
            move_list[index++] = (Move){i, R};
            // DEBUG_PRINT("Move R, hole %d\n", i ); 
        }
        if(nb_graines_T>0) { 
            move_list[index++] =  (Move){i, TB};
            // DEBUG_PRINT("Move TB, hole %d\n", i ); 
            move_list[index++] =  (Move){i, TR};
            // DEBUG_PRINT("Move TR, hole %d\n", i ); 
        }

    } 
    // DEBUG_PRINT("Nb total de moves : %d\n", index);
    return index;
}

int estimation_nb_moves(Board* board) {
    // Estimate the number of possible moves for player
    int count = 0;
    count += board->seed_count / 2 * 4; // Rough estimate based on seed count
    return count;
}

void bot_play(Board* board) {
    // Fonction principale du bot qui choisit et joue un coup
    Move bestMove;
    struct timeval debut, fin;
    gettimeofday(&debut, NULL);
    // exit(0);

    int profondeur = 2;
    // bestMove = decisionMinMax(board, PLAYER, profondeur);
    // fprintf(stderr, "---------------------------------------\n");
    bestMove = decisionAlphaBeta(board, PLAYER, profondeur);
    fprintf(stderr, "Hole index: %d, Seed type: %s\n", bestMove.hole_index, 
        (bestMove.type == R) ? "R" : 
        (bestMove.type == B) ? "B" : 
        (bestMove.type == TR) ? "TR" : "TB");
    if (!is_valid_move(board, bestMove.hole_index, bestMove.type, PLAYER)) {
        fprintf(stderr, "Bot selected an invalid move. Skipping turn.\n"); // Debug message
        exit(EXIT_FAILURE);
        return;
    }
    fprintf(stderr, "Bot selected move: hole %d, type %d\n", bestMove.hole_index, bestMove.type); // Debug message
    log("Bot selected move: hole %d, type %s", bestMove.hole_index, 
        (bestMove.type == R) ? "R" : 
        (bestMove.type == B) ? "B" : 
        (bestMove.type == TR) ? "TR" : "TB");
        
    make_move(board, bestMove.hole_index, bestMove.type, PLAYER);

    printf("%d %s\n", bestMove.hole_index + 1, 
        (bestMove.type == R) ? "R" : 
        (bestMove.type == B) ? "B" : 
        (bestMove.type == TR) ? "TR" : "TB");
    fflush(stdout);

    gettimeofday(&fin, NULL);
    double temps_ms = (fin.tv_sec - debut.tv_sec) * 1000.0 +
                      (fin.tv_usec - debut.tv_usec) / 1000.0;
    // DEBUG_PRINT("Bot move took %.2f ms\n", temps_ms);
    fprintf(stderr, "Bot move took %.2f ms\n", temps_ms);
}