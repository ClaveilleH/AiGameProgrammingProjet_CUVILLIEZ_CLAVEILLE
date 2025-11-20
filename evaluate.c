#include <stdio.h>
#include <stdlib.h>
#include "data.h"
#include "game.h"
#include "bot.h"
#include "evaluate.h"
#include "weights.h"

#include <stdlib.h>
#include <time.h> 

#define VAL_MAX 100000
//On adapte le code du prof pour avoir une esquisse d'évaluation

int check_winning_position(Board* board, int player) {
    int winner;
    if (check_winner(board, &winner)) {
        return (winner == player); // gagnant ?
    }
    return 0; 
}

int check_loosing_position(Board* board, int player) {
    int winner;
    if (check_winner(board, &winner)) {
        return (winner != player); // perdu ?
    }
    return 0;
}

int check_draw_position(Board* board, int player) {
    return check_draw(board);
}


//probablement à changer ??
int h(Board* board, int player) {
    // Example placeholder: difference in score
    int sum = 0;
    if (player) {
        sum += (board->j1_score - board->j2_score) * SCORE_DIF_W;
        DEBUG_PRINT("Value for score difference (player 1): %d\n", (board->j1_score - board->j2_score) * SCORE_DIF_W);
    } else {
        sum += (board->j2_score - board->j1_score) * SCORE_DIF_W;
        DEBUG_PRINT("Value for score difference (player 2): %d\n", (board->j2_score - board->j1_score) * SCORE_DIF_W);
    }
    
    // favorise un plateau ou il y a moins de graines
    if (0) {
        for (int i = 0; i < MAX_HOLES; i++) { // On compte les graines dans les trous 
            Hole* hole = &board->holes[i];
            sum -= (hole->R + hole->B + hole->T) * TOTAL_SEED_W; // Total seeds in holes
        }
    }
    DEBUG_PRINT("Value after total seeds (player): %d\n", sum);
    int total_seeds_player = 0;
    for (int i = player; i < MAX_HOLES; i +=2) {
        Hole* hole = &board->holes[i];
        total_seeds_player += (hole->R + hole->B + hole->T);
    }
    DEBUG_PRINT("Total seeds for player %d: %d\n", player, total_seeds_player);
    sum += total_seeds_player * TOTAL_SEED_PLAYER_W; // Favoriser les trous du joueur
    int total_seeds_opponent = 0;
    for (int i = 1 - player; i < MAX_HOLES; i +=2) {
        Hole* hole = &board->holes[i];
        total_seeds_opponent += (hole->R + hole->B + hole->T);
    }
    DEBUG_PRINT("Total seeds for opponent of player %d: %d\n", player, total_seeds_opponent);
    sum -= total_seeds_opponent * TOTAL_SEED_OPP_W; // Désavantager les trous de l'adversaire



    // for (int i = (player); i < MAX_HOLES; i = i + 2) {
    //     Hole* hole = &board->holes[i];
    //     sum -= (hole->R + hole->B + hole->T) * TOTAL_SEED_W; // Total seeds in holes
    // }
    // for (int i = 0; i < MAX_HOLES; i++) {
    //     Hole* hole = &board->holes[i];
    //     int total_seeds = get_total_seeds(hole);
    //     if (total_seeds == 2 || total_seeds == 3) {
    //         sum += 5; // Favoriser les trous avec 2 ou 3 graines
    //     }
    // }
    return sum;
}


Move decisionMinMax ( Board* board, int player, int pmax ){
    /*
    Decide the best move of J in position e
    :pmax: maximal depth
    
    */
    Move moves[MAX_HOLES/2*4];
    int n_moves = get_move_list(board, moves);

    int bestVal;
    Move bestMove = moves[0];

    //si player 1 on maximise
    if (player){
        bestVal = -VAL_MAX;
        // Pour chaque move possible du joueur 
        for (int i = 0; i < n_moves; i++){
            Board new_board = *board;
            make_move(&new_board, moves[i].hole_index, moves[i].type);

            int value = minMaxValue(&new_board, player, 0, pmax - 1);

            DEBUG_PRINT("Move %d: value = %d\n", i, value);
            if (value > bestVal) {
                bestVal = value;
                bestMove = moves[i];
            }
        }
    } else{ //joueur 2
        bestVal = VAL_MAX;
        // Pour chaque move possible du joueur 
        for (int i = 0; i < n_moves; i++){
            Board new_board = *board;
            make_move(&new_board, moves[i].hole_index, moves[i].type);

            int value = minMaxValue(&new_board, player, 0, pmax - 1);

            DEBUG_PRINT("Move %d: value = %d\n", i, value);
            if (value < bestVal) {
                bestVal = value;
                bestMove = moves[i];
            }
        }

    }
    
    return bestMove;
}



int minMaxValue (Board* board, int player, int isMax, int pmax) {
    // Compute the value of  for the player depending whether e IsMax or not
    // pmax is the maximal depth
    if (check_winning_position(board, player)) return VAL_MAX;
    if (check_loosing_position (board, player)) return(-VAL_MAX);
    if (check_draw_position(board, player)) return(0);
    if (pmax==0)  return h(board, player);

    Move moves[MAX_HOLES/2*4];
    int n_moves = get_move_list(board, moves);

    int bestVal = isMax ? -VAL_MAX : VAL_MAX;

    // Pour chaque move possible du joueur 
    for (int i = 0; i < n_moves; i++){
        Board new_board = *board;
        make_move(&new_board, moves[i].hole_index, moves[i].type);

        int value = minMaxValue(&new_board, 1 - player, 1 - isMax, pmax - 1);
        if (isMax)
            bestVal = (value > bestVal) ? value : bestVal;
        else
            bestVal = (value < bestVal) ? value : bestVal;
    }

    return bestVal;
}


Move decisionAlphaBeta ( Board* board, int player, int pmax ){
    // Decide the best move to play for player with the board
    Move moves[MAX_HOLES/2*4];
    int n_moves = get_move_list(board, moves);

    int alpha = -VAL_MAX;
    int beta = VAL_MAX;
    Move bestMove = moves[0];

    for (int i = 0; i < n_moves; i++) {
        Board new_board = *board;
        make_move(&new_board, moves[i].hole_index, moves[i].type);
        int val = alphaBetaValue(&new_board, (1 - player), alpha, beta, 0, pmax-1);
        if (val>alpha) {
            alpha = val  ;   
            bestMove = moves[i];   
        }
    } 
    return bestMove;
}


int alphaBetaValue (Board* board, int player, int alpha, int beta, int isMax, int pmax){
    // Compute the value e for the player J depending on e.pmax is the maximal depth
    // pmax is the maximal depth
    if (check_winning_position(board, player)) return VAL_MAX;
    if (check_loosing_position (board, player)) return(-VAL_MAX);
    if (check_draw_position(board, player)) return(0);
    if (pmax==0)  return h(board, player);

    Move moves[MAX_HOLES/2*4];
    int n_moves = get_move_list(board, moves);

    if (isMax){
        for (int i = 0; i < n_moves; i++){
            Board new_board = *board;// copie par valeur
            make_move(&new_board, moves[i].hole_index, moves[i].type);

            int val = alphaBetaValue(&new_board, (1 - player), alpha, beta, 1 - isMax, pmax - 1);
            if (val > alpha) alpha = val;
            if (alpha >= beta) break; // Beta cut
        }
        return alpha;
    }
    // Min 
    else{
        for (int i = 0; i < n_moves; i++){
            Board new_board = *board;
            make_move(&new_board, moves[i].hole_index, moves[i].type);
            int val = alphaBetaValue(&new_board, (1 - player), alpha, beta, 1 - isMax, pmax - 1);
            if (val < beta) beta = val;
            if (alpha >= beta) break; // Alpha cut
        }
        return beta;
    }
}
