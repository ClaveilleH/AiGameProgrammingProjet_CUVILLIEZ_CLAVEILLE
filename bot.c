#include "data.h"
#include "game.h"
#include "bot.h"
#include "logger.h"
#include "evaluate.h"


#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>

#define MAX_DEPTH 20
#define DISPO_TIME 2000.0 // Temps disponible en ms pour le bot
#define EVAL_TIME  0.002 // Temps estimé pour l'évaluation en ms

void send_move(int hole_index, SeedType type) {
    const char *type_str =
        (type == R)  ? "R"  :
        (type == B)  ? "B"  :
        (type == TR) ? "TR" : "TB";

    printf("%d %s\n", hole_index + 1, type_str);
    fflush(stdout);
}


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


int get_sorted_move_list(Board* board, Move* move_list, int player, Move previousBestMove) { 
    if (previousBestMove.hole_index == 255) {
        return get_move_list(board, move_list, player);
    }

    int index = 0;

    // Placer le meilleur coup précédent en premier
    move_list[index++] = previousBestMove;
    
    // DEBUG_PRINT("Possible moves : \n"); 
    // Function to get possible moves
    for (int i = player ; i < MAX_HOLES; i+=2) { 
        // Check each hole for possible moves 
        Hole* hole = get_hole(board, i); 
        int nb_graines_R = hole->R; 
        int nb_graines_B = hole->B; 
        int nb_graines_T = hole->T; 
        
        // Ajouter les coups, en évitant de dupliquer previousBestMove
        if(nb_graines_B>0) {
            Move move = (Move){i, B};
            if (!(move.hole_index == previousBestMove.hole_index && move.type == previousBestMove.type)) {
                move_list[index++] = move;
            }
        }
        if(nb_graines_R>0) {
            Move move = (Move){i, R};
            if (!(move.hole_index == previousBestMove.hole_index && move.type == previousBestMove.type)) {
                move_list[index++] = move;
            }
        }
        if(nb_graines_T>0) { 
            Move move_tb = (Move){i, TB};
            if (!(move_tb.hole_index == previousBestMove.hole_index && move_tb.type == previousBestMove.type)) {
                move_list[index++] = move_tb;
            }
            Move move_tr = (Move){i, TR};
            if (!(move_tr.hole_index == previousBestMove.hole_index && move_tr.type == previousBestMove.type)) {
                move_list[index++] = move_tr;
            }
        }

    } 
    // DEBUG_PRINT("Nb total de moves : %d\n", index);
    return index;
}


int estimation_nb_moves(Board* board) {
    // Estimate the number of possible moves for player
    // int count = 0;
    // count += board->seed_count / 2 * 4; // Rough estimate based on seed count
    // return count;
    Move poub[MAX_HOLES/2*4];
    int moves_count_player_1 = get_move_list(board, poub, 0);
    int moves_count_player_2 = get_move_list(board, poub, 1);
    int average_moves = (moves_count_player_1 + moves_count_player_2) / 2;
    return average_moves;
}

int eval_profondeur(Board* board) {
    // Estimation du branching factor
    //fprintf(stderr, "[DEPTH ESTIMATION] Estimating branching factor...\n");
    int b_estime = estimation_nb_moves(board);
    //fprintf(stderr, "[DEPTH ESTIMATION] Estimated branching factor: %d\n", b_estime);
    // double b_effective = sqrt((double)b_estime);
    double b_effective = b_estime/3;
    //fprintf(stderr, "[DEPTH ESTIMATION] Effective branching factor (sqrt): %.2f\n", b_effective);
    // Sécurité minimale
    if (b_effective <= 1) {
        return 1;
    }

    // Temps total disponible en nombre d'évaluations
    double max_evals = DISPO_TIME / EVAL_TIME;
    //fprintf(stderr, "[DEPTH ESTIMATION] Estimated max evaluations: %.2f\n", max_evals);
    if (max_evals < 1.0) {
        return 0;
    }
    max_evals *= 0.9; // marge de sécurité de 10%

    // Calcul théorique de la profondeur
    int depth = (int)(log2(max_evals) / log2((double)b_effective));
    //fprintf(stderr, "[DEPTH ESTIMATION] Theoretical depth: %d\n", depth);

    // Bornes de sécurité
    if (depth < 1) depth = 1;
    if (depth > MAX_DEPTH) depth = MAX_DEPTH;

    // fprintf(stderr,
    //     "[DEPTH ESTIMATION] b=%d, max_evals=%.2f => depth=%d\n",
    //     b_estime, max_evals, depth
    // );

    return depth;
}

void bot_play(Board* board) {
    // Fonction principale du bot qui choisit et joue un coup
    Move bestMove;
    struct timeval debut, fin;
    gettimeofday(&debut, NULL);
    // exit(0);

    // int profondeur = 6;
    // int profondeur = eval_profondeur(board);
    // bestMove = decisionMinMax(board, PLAYER, profondeur);
    // fprintf(stderr, "---------------------------------------\n");
    // bestMove = decisionAlphaBeta(board, PLAYER, profondeur);
    int min_depth = eval_profondeur(board) - 2;
    if (min_depth < 1) min_depth = 1;
    int max_depth = MAX_DEPTH;
    //fprintf(stderr, "[BOT PLAY] Chosen depth range: %d to %d\n", min_depth, max_depth);
    bestMove = iterativeDeepeningAlphaBeta(board, PLAYER, min_depth, max_depth); // 100 ms de marge
    // fprintf(stderr, "Hole index: %d, Seed type: %s\n", bestMove.hole_index, 
    //     (bestMove.type == R) ? "R" : 
    //     (bestMove.type == B) ? "B" : 
    //     (bestMove.type == TR) ? "TR" : "TB");
    if (!is_valid_move(board, bestMove.hole_index, bestMove.type, PLAYER)) {
        fprintf(stderr, "Bot selected an invalid move. Skipping turn.\n"); // Debug message
        _log("Bot selected an invalid move: hole %d, type %s", bestMove.hole_index, 
            (bestMove.type == R) ? "R" : 
            (bestMove.type == B) ? "B" : 
            (bestMove.type == TR) ? "TR" : "TB");
        exit(EXIT_FAILURE);
        return;
    }
    // fprintf(stderr, "Bot selected move: hole %d, type %d\n", bestMove.hole_index, bestMove.type); // Debug message
    // _log("Bot selected move: hole %d, type %s", bestMove.hole_index, 
        // (bestMove.type == R) ? "R" : 
        // (bestMove.type == B) ? "B" : 
        // (bestMove.type == TR) ? "TR" : "TB");
        
    make_move(board, bestMove.hole_index, bestMove.type, PLAYER);
    //send_move(bestMove.hole_index, bestMove.type);

    printf("%d %s\n", bestMove.hole_index + 1, 
        (bestMove.type == R) ? "R" : 
        (bestMove.type == B) ? "B" : 
        (bestMove.type == TR) ? "TR" : "TB");
    fflush(stdout);

    gettimeofday(&fin, NULL);
    double temps_ms = (fin.tv_sec - debut.tv_sec) * 1000.0 +
                      (fin.tv_usec - debut.tv_usec) / 1000.0;
    // DEBUG_PRINT("Bot move took %.2f ms\n", temps_ms);
    //fprintf(stderr, "Bot move took %.2f ms\n", temps_ms);
}