#include <stdio.h>
#include <stdlib.h>
#include "data.h"
#include "game.h"
#include "bot.h"
#include "evaluate.h"
#include "logger.h"

#include "weights.h"

#include <stdlib.h>
#include <time.h> 

#define VAL_MAX 100000
//On adapte le code du prof pour avoir une esquisse d'évaluation

#define HEURISTIC evaluate
// #define HEURISTIC h
//#define HEURISTIC heuristic_evaluation

int check_winning_position(Board* board, int player) {
    int winner;
    if (check_winner(board, &winner)) {
        // fprintf(stderr, "nb : %d", board->j1_score);
        // fprintf(stderr, "nb : %d", board->j2_score);
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

int check_draw_position(Board* board) {
    return check_draw(board);
}


int heuristic_evaluation(Board* board, int player) {
    // l'heuristique d'évaluation basée sur ce qu'a compris chatgpt de la these
    int sum = 0;
    int h1 = 0; // plus a gauche, pas utile on remplace par Nombre total de coups possibles
    int h2 = 0;
    int h3 = 0;
    int h4 = 0;
    int h5 = 0;
    int h6 = 0;

    /*====H1====*/
    // Nombre total de coups possibles
    // nombre de coups possibles pour moi−nombre de coups possibles pour l’adversaire
    // fprintf(stderr, "H1\n");
    Move move[MAX_HOLES/2*4];
    int my_moves = get_move_list(board, move, player);
    int opp_moves = get_move_list(board, move, 1 - player);
    h1 = my_moves - opp_moves;

    /*====H2====*/
    // (mes graines)−(graines adverses)
    // fprintf(stderr, "H2\n");
    int my_seeds = 0;
    int opp_seeds = 0;
    for (int i = 0; i < MAX_HOLES; i++) {
        Hole* hole = &board->holes[i];
        if (i % 2 == player) { // Trous du joueur
            my_seeds += hole->R + hole->B + hole->T;
        } else { // Trous de l'adversaire
            opp_seeds += hole->R + hole->B + hole->T;
        }
    }
    h2 = my_seeds - opp_seeds;

    /*====H3====*/
    // mes trous non vides − ceux de l’adversaire
    // fprintf(stderr, "H3\n");
    int my_non_empty = 0;
    int opp_non_empty = 0;
    for (int i = 0; i < MAX_HOLES; i++) {
        Hole* hole = &board->holes[i];
        if (i % 2 == player) { // Trous du joueur
            if (get_total_seeds(hole) > 0) {
                my_non_empty++;
            }
        } else { // Trous de l'adversaire
            if (get_total_seeds(hole) > 0) {
                opp_non_empty++;
            }
        }
    }
    h3 = my_non_empty - opp_non_empty;

    /*====H4====*/
    // mes captures − captures adverses

    /*====H5====*/
    // 

    /*====H6====*/
    // potentiel de capture de l’adversaire au prochain coup



    sum += h1 * weights.H1_W;
    sum += h2 * weights.H2_W;
    sum += h3 * weights.H3_W;
    sum += h4 * weights.H4_W;
    sum += h5 * weights.H5_W;
    sum += h6 * weights.H6_W;
    return sum;
}

//probablement à changer ??
int h(Board* board, int player) {
    // Example placeholder: difference in score
    int sum = 0;
    int score_dif = get_score(board, PLAYER) - get_score(board, 1 - PLAYER);
    // printf("PLAYER %d score: %d, OPP score: %d, dif: %d\n", PLAYER, get_score(board, PLAYER), get_score(board, 1 - PLAYER), score_dif);

    sum += score_dif * weights.SCORE_DIF_W;
    return sum;

    // favorise un plateau ou il y a moins de graines
    if (0) {
        for (int i = 0; i < MAX_HOLES; i++) { // On compte les graines dans les trous 
            Hole* hole = &board->holes[i];
            sum -= (hole->R + hole->B + hole->T) * weights.TOTAL_SEED_W; // Total seeds in holes
        }
    }
    // DEBUG_PRINT("Value after total seeds (player): %d\n", sum);
    int total_seeds_player = 0;
    for (int i = player; i < MAX_HOLES; i +=2) {
        Hole* hole = &board->holes[i];
        total_seeds_player += (hole->R + hole->B + hole->T);
    }
    // DEBUG_PRINT("Total seeds for player %d: %d\n", player, total_seeds_player);
    sum += total_seeds_player * weights.TOTAL_SEED_PLAYER_W; // Favoriser les trous du joueur
    int total_seeds_opponent = 0;
    for (int i = 1 - player; i < MAX_HOLES; i +=2) {
        Hole* hole = &board->holes[i];
        total_seeds_opponent += (hole->R + hole->B + hole->T);
    }
    // DEBUG_PRINT("Total seeds for opponent of player %d: %d\n", player, total_seeds_opponent);
    sum -= total_seeds_opponent * weights.TOTAL_SEED_OPP_W; // Désavantager les trous de l'adversaire



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


// maximum seeds in a single hole
int h1(Board* board, int player) {
    int max_seeds = 0;
    for (int i = player; i < MAX_HOLES; i += 2) {
        int total = board->holes[i].R + board->holes[i].B + board->holes[i].T;
        if (total > max_seeds) max_seeds = total;
    }
    return max_seeds;
}

// total seeds on player's side
int h2(Board* board, int player) {
    int total = 0;
    for (int i = player; i < MAX_HOLES; i += 2)
        total += board->holes[i].R + board->holes[i].B + board->holes[i].T;
    return total;
}

// number of available moves
int h3(Board* board, int player) {
    Move moves[MAX_HOLES / 2 * 4];
    return get_move_list(board, moves, player);
}

// score advantage (player vs opponent)
int h4(Board* board, int player) {
    return get_score(board, player) - get_score(board, 1 - player);
}

// opponent's score to minimize
int h5(Board* board, int player) {
    return get_score(board, 1 - player);
}

// avoid leaving 1 or 2 seeds in holes  = vulnerable holes
int h6(Board* board, int player) {
    for (int i = player; i < MAX_HOLES; i += 2) {
        int total = board->holes[i].R + board->holes[i].B + board->holes[i].T;
        if (total == 1 || total == 2) return 0; // vulnerable
    }
    return 1; // safe
}

// starve the opponent 
int h7(Board* board, int player) {
    int total = 0;
    for (int i = 1 - player; i < MAX_HOLES; i += 2)
        total += board->holes[i].R + board->holes[i].B + board->holes[i].T;
    return -total;
}

// possibility to capture
int h8(Board* board, int player) {
    int potential = 0;
    Move moves[MAX_HOLES / 2 * 4];
    int n_moves = get_move_list(board, moves, player);

    for (int i = 0; i < n_moves; i++) {
        Board tmp = *board;
        make_move(&tmp, moves[i].hole_index, moves[i].type, player);
        potential += get_score(&tmp, player) - get_score(board, player);
    }
    return potential;
}


// keep red seeds and give blue to the oponent
int h9(Board* board, int player) {
    int score = 0;
    for (int i = player; i < MAX_HOLES; i += 2) score += board->holes[i].R;
    for (int i = 1 - player; i < MAX_HOLES; i += 2) score += board->holes[i].B;
    return score;
}

// fewer moves for the opponent
int h10(Board* board, int player) {
    Move moves[MAX_HOLES / 2 * 4];
    int n_moves = get_move_list(board, moves, 1 - player);
    return -n_moves;
}



int evaluate(Board* board, int player) {
    int value = 0;

    value += h1(board, player) * weights.H1_W;
    value += h2(board, player) * weights.H2_W;
    value += h3(board, player) * weights.H3_W;
    value += h4(board, player) * weights.H4_W;
    value -= h5(board, player) * weights.H5_W;

    value += h6(board, player) * weights.H6_W;
    value += h7(board, player) * weights.H7_W;
    value += h8(board, player) * weights.H8_W;
    value += h9(board, player) * weights.H9_W;
    value += h10(board, player) * weights.H10_W;
    //value += h11(board, player) * weights.H11_W;
    //value += h12(board, player) * weights.H12_W;

    return value;
}



Move decisionMinMax ( Board* board, int player, int pmax ){
    player = player % 2;
    /*
    Decide the best move of J in position e
    :pmax: maximal depth
    */
    Move moves[MAX_HOLES/2*4];
    int n_moves = get_move_list(board, moves, player);
    
    int bestVal;
    Move bestMove = moves[0];

    //si player 1 on maximise en faite peut être pas car on n'est pas forcément player 1
    if (player == PLAYER){
        bestVal = -VAL_MAX;
        // Pour chaque move possible du joueur 
        for (int i = 0; i < n_moves; i++){
            Board new_board = *board;
            make_move(&new_board, moves[i].hole_index, moves[i].type, player);
            // fprintf(stderr, "Evaluating move %d/%d: hole %d, type %d\n", i+1, n_moves, moves[i].hole_index, moves[i].type);
            int value = minMaxValue(&new_board, player, 0, pmax - 1);

            fprintf(stderr, "(%d/%d): %d,%s > %d : ", i+1, n_moves, moves[i].hole_index, 
                (moves[i].type == R) ? "R" : 
                (moves[i].type == B) ? "B" : 
                (moves[i].type == TR) ? "TR" : "TB", value);
            MoveList* current = NULL;
            while (current != NULL && current->moves != NULL) {
                fprintf(stderr, "[%d,%s] ", current->moves->hole_index, 
                    (current->moves->type == R) ? "R" : 
                    (current->moves->type == B) ? "B" : 
                    (current->moves->type == TR) ? "TR" : "TB");
                current = current->next;
            } 
            fprintf(stderr, "\n");
            // DEBUG_PRINT("Move %d: value = %d\n", i, value);
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
            make_move(&new_board, moves[i].hole_index, moves[i].type, player);
            fprintf(stderr, "Evaluating move %d/%d: hole %d, type %d\n", i+1, n_moves, moves[i].hole_index, moves[i].type);
            // int value = minMaxValue(&new_board, player, 0, pmax - 1);
            int value = minMaxValue(&new_board, (1 -player), 0, pmax - 1);

            // DEBUG_PRINT("Move %d: value = %d\n", i, value);
            if (value < bestVal) {
                bestVal = value;
                bestMove = moves[i];
            }
        }

    }
    fprintf(stderr, "Best move chosen : hole %d, type %d with value %d\n", bestMove.hole_index, bestMove.type, bestVal);
    return bestMove;
}



int minMaxValue (Board* board, int player, int isMax, int pmax) {
    player = player % 2;

    // Compute the value of  for the player depending whether e IsMax or not
    // pmax is the maximal depth
    if (check_winning_position(board, player)) return VAL_MAX ; 
    if (check_loosing_position (board, player)) return(-VAL_MAX) ; 
    if (check_draw_position(board)) return(0);
    if (pmax==0)  return HEURISTIC(board, player);
    // if (pmax==0)  return evaluate(board, player);

    Move moves[MAX_HOLES/2*4];
    int n_moves = get_move_list(board, moves, player);

    int bestVal = isMax ? -VAL_MAX : VAL_MAX;

    // Pour chaque move possible du joueur 
    for (int i = 0; i < n_moves; i++){
        Board new_board = *board;
        make_move(&new_board, moves[i].hole_index, moves[i].type, player);

        int value = minMaxValue(&new_board, (1 - player), (1 - isMax), (pmax - 1));
        if (isMax)
            bestVal = (value > bestVal) ? value : bestVal;
        else
            bestVal = (value < bestVal) ? value : bestVal;
    }

    return bestVal;
}


Move decisionAlphaBeta ( Board* board, int player, int pmax ){
    player = player % 2; 
    // on ajoute de l'aléatoire pour les moves avec la même valeur
    Move bestMoves[MAX_HOLES/2*4];
    int cptBestMoves = 0;
    // Decide the best move to play for player with the board
    Move moves[MAX_HOLES/2*4];
    int n_moves = get_move_list(board, moves, player);
    int alpha = -VAL_MAX;
    int beta = VAL_MAX;
    Move bestMove;// = moves[0];
    MoveList* bestMoveList = NULL;
    MoveList* moveList = NULL;
    for (int i = 0; i < n_moves; i++) {
        Board new_board = *board;
        moveList = malloc(sizeof(MoveList));
        moveList->moves = &moves[i];
        moveList->next = NULL;
        
        /* Create chain of pmax nodes for the principal variation */
        MoveList* current = moveList;
        for (int depth = 0; depth < pmax; depth++) {
            current->next = malloc(sizeof(MoveList));
            current->next->moves = NULL;
            current->next->next = NULL;
            current = current->next;
        }
        
        make_move(&new_board, moves[i].hole_index, moves[i].type, player);
        int val = alphaBetaValue(&new_board, (1 - player), alpha, beta, 1, 0, pmax-1, moveList->next);
        // int val = alphaBetaValue(&new_board, player, alpha, beta, 0, pmax-1, moveList->next);
        if (val>alpha) {
            alpha = val  ;   
            cptBestMoves = 0;
            bestMoves[cptBestMoves++] = moves[i];

            /* Free the previous bestMoveList if it exists (replaced by new best) */
            if (bestMoveList != NULL) {
                MoveList* temp = bestMoveList;
                while (temp != NULL) {
                    MoveList* next = temp->next;
                    if (temp->moves != NULL && !(temp->moves >= moves && temp->moves < moves + n_moves)) {
                        /* Free only if it's not pointing to stack array */
                        free(temp->moves);
                    }
                    free(temp);
                    temp = next;
                }
            }
            bestMoveList = moveList; /* Keep this moveList as the best */
        
        } 
        else if (val == alpha ){
            bestMoves[cptBestMoves++] = moves[i];
        }
        else {
            /* This moveList is not the best, free the entire chain */
            MoveList* temp = moveList;
            while (temp != NULL) {
                MoveList* next = temp->next;
                if (temp->moves != NULL && !(temp->moves >= moves && temp->moves < moves + n_moves)) {
                    /* Free only if it's not pointing to stack array */
                    free(temp->moves);
                }
                free(temp);
                temp = next;
            }
        }
        moveList = NULL;
    }
    DEBUG_PRINT("Best move chosen: hole %d, type %s with value %d\n", bestMove.hole_index, 
        (bestMove.type == R) ? "R" : 
        (bestMove.type == B) ? "B" : 
        (bestMove.type == TR) ? "TR" : "TB", alpha);
    log("Best move chosen: [%d,%s] with value %d", bestMove.hole_index + 1, 
        (bestMove.type == R) ? "R" : 
        (bestMove.type == B) ? "B" : 
        (bestMove.type == TR) ? "TR" : "TB", alpha);
    // fprintf(stderr, "Best move chosen: hole %d, type %d with value %d\n", bestMove.hole_index, bestMove.type, alpha);
    // fprintf(stderr, "Best move chosen: hole %d, type %d with value %d\n\n", bestMove.hole_index, bestMove.type, alpha);
    if (DEBUG) {
        fprintf(stderr, "Nexts bests moves : ");
        MoveList* current = bestMoveList;
        while (current != NULL && current->moves != NULL) {
            fprintf(stderr, "[%d,%s] ", current->moves->hole_index, 
                (current->moves->type == R) ? "R" : 
                (current->moves->type == B) ? "B" : 
                (current->moves->type == TR) ? "TR" : "TB");
            current = current->next;
        }
        fprintf(stderr, "\n");
        fflush(stderr);
    }
    
    
    /* Free the bestMoveList now that we've finished using it */
    if (bestMoveList != NULL) {
        MoveList* temp = bestMoveList;
        while (temp != NULL) {
            MoveList* next = temp->next;
            if (temp->moves != NULL && !(temp->moves >= moves && temp->moves < moves + n_moves)) {
                /* Free only if it's not pointing to stack array */
                free(temp->moves);
            }
            free(temp);
            temp = next;
        }
    }
    if (board->seed_count > 80) { //début de partie après on retire l'aléatoire
        bestMove = bestMoves[rand() % cptBestMoves];
    }
    else{
        bestMove=bestMoves[0];
    }
    return bestMove;
}


int alphaBetaValue (Board* board, int player, int alpha, int beta, int isMax, int depth, int pmax, MoveList* moveList) {
    player = player % 2;

    // Compute the value e for the player J depending on e.pmax is the maximal depth
    // pmax is the maximal depth
    if (check_winning_position(board, player)) return VAL_MAX -depth; //éviter l'effet d'horizon on minimise la distance au gain
    if (check_loosing_position (board, player)) return(-VAL_MAX +depth); // on mximise la distance à la défaite
    if (check_draw_position(board)) return(0+depth);
    if (pmax==0)  return HEURISTIC(board, player);

    Move moves[MAX_HOLES/2*4];
    int n_moves = get_move_list(board, moves, player);

    MoveList* currentMoveList = moveList;
    
    if (isMax){
        for (int i = 0; i < n_moves; i++){
            Board new_board = *board;// copie par valeur
            make_move(&new_board, moves[i].hole_index, moves[i].type, player);
            // fprintf(stderr, "      max(%d/%d): hole %d, type %d\n", i+1, n_moves, moves[i].hole_index, moves[i].type);
            /* Pass next element only if currentMoveList is non-NULL */
            int val = alphaBetaValue(&new_board, (1 - player), alpha, beta, 1 - isMax, depth + 1,pmax - 1, currentMoveList ? currentMoveList->next : NULL);
            if (val > alpha) {
                alpha = val;
                /* Only record move if we have a MoveList node to write into */
                if (currentMoveList) {
                    if (currentMoveList->moves == NULL) {
                        currentMoveList->moves = malloc(sizeof(Move));
                    }
                    *(currentMoveList->moves) = moves[i];
                }
            }
            if (alpha >= beta) break; // Beta cut
        }
        if (currentMoveList) {
            if (currentMoveList->moves) {
                free(currentMoveList->moves);
                currentMoveList->moves = NULL;
            }
        }
        // free(currentMoveList->next);
        // free(currentMoveList);
        return alpha;
    }
    // Min 
    else{
        for (int i = 0; i < n_moves; i++){
            Board new_board = *board;
            make_move(&new_board, moves[i].hole_index, moves[i].type, player);
            /* Pass next element only if currentMoveList is non-NULL */
            int val = alphaBetaValue(&new_board, (1 - player), alpha, beta, 1 - isMax, depth +1, pmax - 1, currentMoveList ? currentMoveList->next : NULL);
            // fprintf(stderr, "      min(%d/%d): %d, %s -> %d\n", i+1, n_moves, moves[i].hole_index, 
            //     (moves[i].type == R) ? "R" : 
            //     (moves[i].type == B) ? "B" : 
            //     (moves[i].type == TR) ? "TR" : "TB", val);
            if (val < beta) {
                beta = val;
                /* Only record move if we have a MoveList node to write into */
                if (currentMoveList) {
                    if (currentMoveList->moves == NULL) {
                        currentMoveList->moves = malloc(sizeof(Move));
                    }
                    *(currentMoveList->moves) = moves[i];
                }
            }
            if (alpha >= beta) break; // Alpha cut
        }
        if (currentMoveList) {
            if (currentMoveList->moves) {
                free(currentMoveList->moves);
                currentMoveList->moves = NULL;
            }
        }
        // free(currentMoveList->next);
        // free(currentMoveList);
        return beta;
    }
}