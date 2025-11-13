#include "data.h"
#include "game.h"
#include "bot.h"
#include "logger.h"
#include "evaluate.h"


#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

int get_move_list(Board* board, Move* move_list) { 
 
    int index = 0;
    DEBUG_PRINT("Possible moves : \n"); 
    // Function to get possible moves
    for (int i = PLAYER ; i < MAX_HOLES; i+=2) { 
        // Check each hole for possible moves 
        Hole* hole = get_hole(board, i); 
        int nb_graines_R = hole->R; 
        int nb_graines_B = hole->B; 
        int nb_graines_T = hole->T; 
        if(nb_graines_B>0) {
            move_list[index++] = (Move){i, B};
            DEBUG_PRINT("Move B, hole %d\n", i ); 
        }
        if(nb_graines_R>0) {
            move_list[index++] = (Move){i, R};
            DEBUG_PRINT("Move R, hole %d\n", i ); 
        }
        if(nb_graines_T>0) { 
            move_list[index++] =  (Move){i, TB};
            DEBUG_PRINT("Move TB, hole %d\n", i ); 
            move_list[index++] =  (Move){i, TR};
            DEBUG_PRINT("Move TR, hole %d\n", i ); } 
        } 
    DEBUG_PRINT("Nb total de moves : %d\n", index);
    return index;
}

void bot_play(Board* board) {
    // Fonction principale du bot qui choisit et joue un coup
    Move bestMove;
    struct timeval debut, fin;
    gettimeofday(&debut, NULL);

    // bestMove = decisionMinMax(board, PLAYER, 3);
    bestMove = decisionAlphaBeta(board, PLAYER, 3);
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
    make_move(board, bestMove.hole_index, bestMove.type);
    printf("%d %s\n", bestMove.hole_index + 1, 
        (bestMove.type == R) ? "R" : 
        (bestMove.type == B) ? "B" : 
        (bestMove.type == TR) ? "TR" : "TB");
    fflush(stdout);

    gettimeofday(&fin, NULL);
    double temps_ms = (fin.tv_sec - debut.tv_sec) * 1000.0 +
                      (fin.tv_usec - debut.tv_usec) / 1000.0;
    DEBUG_PRINT("Bot move took %.2f ms\n", temps_ms);
    fprintf(stderr, "Bot move took %.2f ms\n", temps_ms);

    return; // -----------------------------------------------------------------
    // Simple bot logic: choose random move till a valid one is found
    int hole_index;
    SeedType type;
    int valid_move = 0;
    while (!valid_move) {
        hole_index = (rand() % (MAX_HOLES / 2)) * 2 + ( PLAYER);
        int type_choice = rand() % 4;
        switch (type_choice) {
            case 0:
                type = R;
                break;
            case 1:
                type = B;
                break;
            case 2:
                type = TR;
                break;
            case 3:
                type = TB;
                break;
            default:
                type = R; // Pour eviter un warning, mais ne devrait jamais arriver
                break;
        }
        if (is_valid_move(board, hole_index, type, PLAYER)) {
            valid_move = 1;
            DEBUG_PRINT("not a valid move\n");
        }
    }
    if (COMPETE) {
        printf("%d %s\n", hole_index + 1, 
            (type == R) ? "R" : 
            (type == B) ? "B" : 
            (type == TR) ? "TR" : "TB");
        // fflush(stdout);
        fflush(stdout);
    } else {
        COMPETE_PRINT("Bot(%d) chooses hole %d with seed type %d\n", PLAYER, hole_index, type);

        printf("%d %s\n", hole_index + 1, 
            (type == R) ? "R" : 
            (type == B) ? "B" : 
            (type == TR) ? "TR" : "TB");
    }
    log("Bot chooses hole %d with seed type %s --> %d %s", hole_index, (type == R) ? "R" : 
        (type == B) ? "B" : 
        (type == TR) ? "TR" : "TB", 
        hole_index + 1, (type == R) ? "R" : 
        (type == B) ? "B" : 
        (type == TR) ? "TR" : "TB");

    // make_move(board, 15, 3);
    make_move(board, hole_index, type);
}