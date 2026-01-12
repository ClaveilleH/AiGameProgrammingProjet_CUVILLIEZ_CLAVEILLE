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
#include <sys/time.h>
#include <math.h>


#define VAL_MAX 100000
#define DISPO_TIME 1000.0 // Temps disponible en ms pour le bot
//On adapte le code du prof pour avoir une esquisse d'évaluation

#define HEURISTIC evaluate
// #define HEURISTIC heuristic_evaluation
// #define HEURISTIC h
//#define HEURISTIC ma_fct_deval



double g_start_time;
int g_timeout;

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



    sum += h1 * H1_W;
    sum += h2 * H2_W;
    sum += h3 * H3_W;
    sum += h4 * H4_W;
    sum += h5 * H5_W;
    sum += h6 * H6_W;
    return sum;
}

//probablement à changer ??
int h(Board* board, int player) {
    // Example placeholder: difference in score
    int sum = 0;
    int score_dif = get_score(board, PLAYER) - get_score(board, 1 - PLAYER);
    // printf("PLAYER %d score: %d, OPP score: %d, dif: %d\n", PLAYER, get_score(board, PLAYER), get_score(board, 1 - PLAYER), score_dif);

    sum += score_dif * SCORE_DIF_W;

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

// keep transparent seeds and give blue to the oponent
int h11(Board* board, int player) {
    int score = 0;
    for (int i = player; i < MAX_HOLES; i += 2) score += board->holes[i].T;
    return score;
}
// ROUGE
// utiliser le rouge seulement quand on veut capturer
// éviter la "famine"
// effacer les trous à 1 ou 2 graines du plateau pour éviter des captures adverses

// BLEU
// viser une capture sur une case précise de l’adversaire
// ne pas modifier tes propres trous
//affamer l’adversaire (le bleu ne nourrit que lui-même)

//TRANSPARENT
//premières graines distribuer
//garder au maximum dans ses trous

//objectif principal affamer l'adversaire, jouer d'abord bleu puis rouge
// ne pas laisser nos trous avec 1 ou 2 graines

int evaluate(Board* board, int player) {
    int value = 0;
    value += h1(board, player) * H1_W;
    value += h2(board, player) * H2_W;
    value += h3(board, player) * H3_W;
    value += h4(board, player) * H4_W;
    value -= h5(board, player) * H5_W;

    value += h6(board, player) * H6_W;
    value += h7(board, player) * H7_W;
    value += h8(board, player) * H8_W;
    value += h9(board, player) * H9_W;
    value += h10(board, player) * H10_W;
    value += h11(board, player) * H11_W;
    return value;
}


int ma_fct_deval(Board* board, int _) {
    int sum = 0;
    // Exemple simple : différence de score
    int score_dif = get_score(board, PLAYER) - get_score(board, 1 - PLAYER);
    // printf("PLAYER %d score: %d, OPP score: %d, dif: %d\n", PLAYER, get_score(board, PLAYER), get_score(board, 1 - PLAYER), score_dif);


    int total_seeds_player = 0;
    int nb_hole_takable_player = 0;
    for (int i = PLAYER; i < MAX_HOLES; i +=2) {
        Hole* hole = &board->holes[i];
        int total = hole->R + hole->B + hole->T;
        if (total == 1 || total ==2) nb_hole_takable_player++;
        total_seeds_player += (total);
    }
    
    int total_seeds_opponent = 0;
    int nb_hole_takable_opponent = 0;
    for (int i = 1 - PLAYER; i < MAX_HOLES; i +=2) {
        Hole* hole = &board->holes[i];
        int total = hole->R + hole->B + hole->T;
        if (total == 1 || total ==2) nb_hole_takable_opponent++;
        total_seeds_opponent += (total);
    }
    int total_seeds = board->seed_count;
    int total_takable = nb_hole_takable_player + nb_hole_takable_opponent;
    int percentage_player = (total_seeds == 0) ? 0 : (total_seeds_player * 100) / total_seeds;
    // le pourcentage de graines qui se trouvent sur le coté du joueur
    int percentage_takable = (total_takable == 0) ? 0 : (nb_hole_takable_player * 100) / total_takable;
    // le pourcentage de trous avec 1 ou 2 graines du coté du joueur

    sum += score_dif * SCORE_DIF_W;
    sum += percentage_player * PRECENTAGE_PLAYER_W;
    sum += (100 - percentage_takable) * PRECENTAGE_TAKABLE_W;
    //afficher le score du moove coisi (la meme pour l'adversaire)
    return sum;
}

Move decisionMinMax ( Board* board, int player, int pmax ){
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
    // Compute the value of  for the player depending whether e IsMax or not
    // pmax is the maximal depth
    if (check_winning_position(board, player)) return VAL_MAX;
    if (check_loosing_position (board, player)) return(-VAL_MAX);
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
    // on ajoute de l'aléatoire pour les moves avec la même valeur
    Move bestMoves[MAX_HOLES/2*4];
    int cptBestMoves = 0;
    // Decide the best move to play for player with the board
    Move moves[MAX_HOLES/2*4];
    int n_moves = get_move_list(board, moves, player);

    int alpha = -VAL_MAX;
    int beta = VAL_MAX;
    Move bestMove = moves[0];
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
        int val = alphaBetaValue(&new_board, (1 - player), alpha, beta,1, 0, pmax-1, moveList->next);
        // int val = alphaBetaValue(&new_board, player, alpha, beta, 0, pmax-1, moveList->next);
        if (val>alpha) {
            alpha = val  ;   
            bestMove = moves[i];
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
    _log("Best move chosen: [%d,%s] with value %d", bestMove.hole_index + 1, 
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
    if (board->seed_count > 80) { //début de partie après on retire l'aléatoire ou coups < 4 par exemple
        bestMove = bestMoves[rand() % cptBestMoves];
    }
    else{
        bestMove=bestMoves[0];
    }
    return bestMove;
}


int alphaBetaValue (Board* board, int player, int alpha, int beta, int isMax, int depth, int pmax, MoveList* moveList) {
    // Compute the value e for the player J depending on e.pmax is the maximal depth
    // pmax is the maximal depth
    // if (check_winning_position(board, player)) return VAL_MAX;
    // if (check_loosing_position (board, player)) return(-VAL_MAX);
    // if (check_draw_position(board)) return(0);
    // on utilise check_end_game, car plus complet
    int winner;
    int end = check_end_game(board, &winner);
    if (end) {
        if (winner == -1) {
            return 0 - 1000; // Draw
        } else if (winner == player) {
            return VAL_MAX -depth; // Win
        } else {
            return -VAL_MAX +depth; // Lose
        }
    }
    // if (pmax==0)  return HEURISTIC(board, player);
    if (pmax==0)  {
        // on mesure le temps d'évaluation (en ms reel)
        int eval = HEURISTIC(board, player);
        return eval;

    }
    Move moves[MAX_HOLES/2*4];
    int n_moves = get_move_list(board, moves, player);
    MoveList* currentMoveList = moveList;
    
    if (isMax){
        for (int i = 0; i < n_moves; i++){
            Board new_board = *board;// copie par valeur
            make_move(&new_board, moves[i].hole_index, moves[i].type, player);
            // fprintf(stderr, "      max(%d/%d): hole %d, type %d\n", i+1, n_moves, moves[i].hole_index, moves[i].type);
            /* Pass next element only if currentMoveList is non-NULL */
            int val = alphaBetaValue(&new_board, (1 - player), alpha, beta, 1 - isMax, depth+1, pmax - 1, currentMoveList ? currentMoveList->next : NULL);
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
            int val = alphaBetaValue(&new_board, (1 - player), alpha, beta, 1 - isMax,depth +1, pmax - 1, currentMoveList ? currentMoveList->next : NULL);
            if (0) _log( "      min(%d/%d): %d, %s -> %d\n", i+1, n_moves, moves[i].hole_index, 
                (moves[i].type == R) ? "R" : 
                (moves[i].type == B) ? "B" : 
                (moves[i].type == TR) ? "TR" : "TB", val);
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



// =============== ITERATIVE DEEPENING ALPHA BETA =============== //



double now_ms() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1e6;
}

Move deepeningDecisionAlphaBeta ( Board* board, int player, int pmax, Move previousBestMove){
    // Decide the best move to play for player with the board
    Move moves[MAX_HOLES/2*4];
    int n_moves = get_sorted_move_list(board, moves, player, previousBestMove);

    fprintf(stderr, "[ID] Starting depth %d with %d moves\n", pmax, n_moves);
    int alpha = -VAL_MAX;
    int beta = VAL_MAX;
    Move bestMove = moves[0];
    // MoveList* moveList = NULL;
    for (int i = 0; i < n_moves && !g_timeout; i++) {
        Board new_board = *board;
        make_move(&new_board, moves[i].hole_index, moves[i].type, player);
        int val = deepeningAlphaBetaValue(&new_board, (1 - player), alpha, beta, 0, pmax-1, 0);
        if (val>alpha) {
            alpha = val;
            bestMove = moves[i];
        } 
    }
    return bestMove;
}


int deepeningAlphaBetaValue (Board* board, int player, int alpha, int beta, int isMax, int pmax, int depth) {
    // Compute the value e for the player J depending on e.pmax is the maximal depth
    // pmax is the maximal depth
    // if (check_winning_position(board, player)) return VAL_MAX;
    // if (check_loosing_position (board, player)) return(-VAL_MAX);
    // if (check_draw_position(board)) return(0);
    // on utilise check_end_game, car plus complet
    if (g_timeout) {
        fprintf(stderr, "[ID] Timeout detected in deepeningAlphaBetaValue\n");
        return 0; // Return a neutral value on timeout
    }
    if (now_ms() - g_start_time > DISPO_TIME) {
        fprintf(stderr, "[ID] Timeout reached in deepeningAlphaBetaValue with %d steps remaining \n", pmax);
        // fprintf(stderr, "[ID] Time taken: %.2f ms, limit was %.2f ms\n", now_ms() - g_start_time, DISPO_TIME);
        g_timeout = 1;
        return 0; // Return a neutral value on timeout
    }
    // a partir de la j'ai rien changé
    int winner;
    int end = check_end_game(board, &winner);
    if (end) {
        if (winner == -1) {
            return 0; // Draw
        } else if (winner == player) {
            return VAL_MAX; // Win
        } else {
            return -VAL_MAX; // Lose
        }
    }
    if (pmax==0) return HEURISTIC(board, player);
    Move moves[MAX_HOLES/2*4];
    int n_moves = get_move_list(board, moves, player);
    if (isMax){
        for (int i = 0; i < n_moves && !g_timeout; i++){
            Board new_board = *board;// copie par valeur
            make_move(&new_board, moves[i].hole_index, moves[i].type, player);
            // fprintf(stderr, "      max(%d/%d): hole %d, type %d\n", i+1, n_moves, moves[i].hole_index, moves[i].type);
            /* Pass next element only if currentMoveList is non-NULL */
            int val = deepeningAlphaBetaValue(&new_board, (1 - player), alpha, beta, 1 - isMax, pmax - 1, depth + 1);
            if (val > alpha) alpha = val; // Update alpha
            if (alpha >= beta) break; // Beta cut
        }
        return alpha;
    }
    // Min 
    else{
        for (int i = 0; i < n_moves && !g_timeout; i++){
            Board new_board = *board;
            make_move(&new_board, moves[i].hole_index, moves[i].type, player);
            /* Pass next element only if currentMoveList is non-NULL */
            int val = deepeningAlphaBetaValue(&new_board, (1 - player), alpha, beta, 1 - isMax, pmax - 1, depth + 1);
            if (0) _log( "      min(%d/%d): %d, %s -> %d\n", i+1, n_moves, moves[i].hole_index, 
                (moves[i].type == R) ? "R" : 
                (moves[i].type == B) ? "B" : 
                (moves[i].type == TR) ? "TR" : "TB", val);
            if (val < beta) {
                beta = val;
            }
            if (alpha >= beta) break; // Alpha cut
        }
        return beta;
    }
}



Move iterativeDeepeningAlphaBeta ( Board* board, int player, int min_depth, int max_depth ){
    g_start_time = now_ms();
    g_timeout = 0;
    fprintf(stderr, "[ID] Starting iterative deepening with time limit %.2f ms\n", DISPO_TIME);
    fprintf(stderr, "[ID] Start time: %.2f ms\n", now_ms() - g_start_time);
    Move bestMove = INVALID_MOVE;

    for (int depth = min_depth; depth <= max_depth; depth++) {
        g_timeout = 0;
        Move currentBestMove = deepeningDecisionAlphaBeta(board, player, depth, bestMove);
        if (g_timeout) {
            fprintf(stderr, "[ID] Timeout at depth %d\n", depth);
            break;
        }
        bestMove = currentBestMove;
        fprintf(stderr, "[ID] depth %d OK\n", depth);
    }

    return bestMove;
}






/*



    if (pmax==0)  {
        // on mesure le temps d'évaluation (en ms reel)
        time_t start_time = clock();
        int eval = HEURISTIC(board, player);
        time_t end_time = clock();
        double time_taken = ((double)(end_time - start_time)) / CLOCKS_PER_SEC * 1000.0;
        // _log("Heuristic evaluation took %.3f ms", time_taken);
        // fprintf(stderr, "[EVAL] evaluation took %f ms\n", time_taken);
        return eval;

    }


*/