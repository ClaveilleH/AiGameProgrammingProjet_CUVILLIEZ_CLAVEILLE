#include <stdio.h>
#include <stdlib.h>
#include "data.h"
#include "game.h"
#include "bot.h"
#include "evaluate.h"

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
    int winner;
    if (!(check_winner(board, &winner))) {
        return 0; // égalité ou pas encore de gagnant
    }
    return 1;
}


//probablement à changer ??
int h(Board* board, int player) {
    // Example placeholder: difference in score
    if (player) return board->j1_score - board->j2_score;
    else return board->j2_score - board->j1_score;
}


Move decisionMinMax ( Board* board, int player, int pmax ){
    // Decide the best move of J in position e
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

            int value = MinMaxValue(&new_board, player, 0, pmax - 1);

            printf("Move %d: value = %d\n", i, value);
            if (value > bestVal) {
                bestVal = value;
                bestMove = moves[i];
            }
        }
    }
    //joueur 2
    else{
        bestVal = VAL_MAX;
        // Pour chaque move possible du joueur 
        for (int i = 0; i < n_moves; i++){
            Board new_board = *board;
            make_move(&new_board, moves[i].hole_index, moves[i].type);

            int value = MinMaxValue(&new_board, player, 0, pmax - 1);

            printf("Move %d: value = %d\n", i, value);
            if (value < bestVal) {
                bestVal = value;
                bestMove = moves[i];
            }
        }

    }
    
    return bestMove;
}



int MinMaxValue (Board* board, int player, int isMax, int pmax) {
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

        int value = MinMaxValue(&new_board, abs(player-1), !isMax, pmax - 1);
        if (isMax)
            bestVal = (value > bestVal) ? value : bestVal;
        else
            bestVal = (value < bestVal) ? value : bestVal;
    }

    return bestVal;
}


Move DecisionAlphaBeta ( Board* board, int player, int pmax ){
    // Decide the best move to play for player with the board
    Move moves[MAX_HOLES/2*4];
    int n_moves = get_move_list(board, moves);

    int alpha = -VAL_MAX;
    int beta = VAL_MAX;
    Move bestMove = moves[0];

    for (int i = 0; i < n_moves; i++){
        Board new_board = *board;
        make_move(&new_board, moves[i].hole_index, moves[i].type);
        int val = AlphaBetaValue(&new_board, abs(player-1), alpha, beta, 0, pmax-1);
        if (val>alpha) {
            alpha = val  ;   
            bestMove = moves[i];   
        }
    } 
    return bestMove;
}


int AlphaBetaValue (Board* board, int player, int alpha, int beta, int isMax, int pmax){
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
            Board new_board = *board;
            make_move(&new_board, moves[i].hole_index, moves[i].type);

            int val = AlphaBetaValue(&new_board, abs(player-1), alpha, beta, 0, pmax - 1);
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
            int val = AlphaBetaValue(&new_board, abs(player-1), alpha, beta, 0, pmax - 1);
            if (val < beta) beta = val;
            if (alpha >= beta) break; // Alpha cut
        }
        return beta;
    }
}
