#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> 

#include "data.h"
#include "game.h"
#include "bot.h"
#include "evaluate.h"
#include "arbre.h"

#include <stdlib.h>
#include <time.h> 

#define VAL_MAX 100000

Board copy_board(const Board* board) {
    Board new_board;
    memcpy(&new_board, board, sizeof(Board)); 
    return new_board;
}


Noeud* create_noeud(int node_id, Board board, Move move, int player, int isMax) {
    Noeud* node = (Noeud*) malloc(sizeof(Noeud));
    node->node_id = node_id;
    node->board = board;
    node->move = move;
    node->player = player;
    node->isMax = isMax;
    node->value = 0;          
    node->children = NULL;
    node->nChildren = 0;
    return node;
}
//nouvelle version 
Noeud* create_children(Noeud* node, int player) {
    Move moves[MAX_HOLES/2 * 4];
    int n_moves = get_move_list(&node->board, moves, player);
    node->nChildren = n_moves;
    if (n_moves > 0)
        node->children = malloc(n_moves * sizeof(Noeud*));
    else {
        node->children = NULL;
        return node;
    }

    for (int i = 0; i < n_moves; ++i) {
        Board child_board = copy_board(&node->board);
        make_move(&child_board, moves[i].hole_index, moves[i].type);

        // Créer le noeud enfant
        Noeud* child = (Noeud*) malloc(sizeof(Noeud));
        child->board = child_board;
        child->move = moves[i];
        child->player = 1 - player;
        child->isMax = (player == PLAYER) ? 0 : 1; // max → min → max → …
        child->value = 0;
        child->children = NULL;
        child->nChildren = 0;

        node->children[i] = child;
    }

    return node;
}


int minimax(Noeud* node, int player, int pmax){
    if (pmax == 0 ||
        check_winning_position(&node->board, player) ||
        check_loosing_position(&node->board, player) ||
        check_draw_position(&node->board)) {
        node->value = evaluate(&node->board, player);
        return node->value;
    }
    create_children(node, player);
    int bestVal;
    //si PLAYER on maximise
    if (player == PLAYER){
        bestVal =  -VAL_MAX;
        for (int i = 0; i < node->nChildren; ++i){
            int val = minimax(node->children[i], 1-player, pmax-1);
            if (val>bestVal) bestVal = val;
        }
    }
    else{
        bestVal =  +VAL_MAX;
        for (int i = 0; i < node->nChildren; ++i){
            int val = minimax(node->children[i], 1-player, pmax-1);
            if (val<bestVal) bestVal = val;
        }
    }
    node->value = bestVal;
    return bestVal;
}


int alpha_beta_minimax(Noeud* node, int alpha, int beta, int player, int pmax){
    if (pmax == 0 ||
        check_winning_position(&node->board, player) ||
        check_loosing_position(&node->board, player) ||
        check_draw_position(&node->board)) {
        node->value = evaluate(&node->board, player);
        return node->value;
    }
    create_children(node, player);
    int bestVal;
    //si PLAYER on maximise
    if (player == PLAYER){
        bestVal =  -VAL_MAX;
        for (int i = 0; i < node->nChildren; ++i){
            int val = alpha_beta_minimax(node->children[i], alpha, beta, 1-player, pmax-1);
            if (val > bestVal) bestVal = val;
            if (bestVal > alpha) alpha = bestVal;
            if (alpha >= beta) break; //pruning
        }
        return bestVal;
    }
    else{
        bestVal = +VAL_MAX;
        for (int i = 0; i < node->nChildren; ++i){
            int val = alpha_beta_minimax(node->children[i], alpha, beta, 1-player, pmax-1);
            if (val < bestVal) bestVal = val;
            if (bestVal < beta) beta = bestVal;
            if (alpha >= beta) break; //pruning
        }
        return bestVal;
    }
}



//ancienne version

Noeud* build_tree(const Board* board, int player, int isMax, int pmax, int* node_counter) {
    Board node_board = copy_board(board);
    Noeud* node = create_noeud((*node_counter)++, node_board, (Move){-1,0}, player, isMax);

    Move moves[MAX_HOLES/2*4];
    int n_moves = get_move_list(&node->board, moves, player);

    printf("[build] node=%d player=%d isMax=%d pmax=%d n_moves=%d j1=%d j2=%d\n",
           node->node_id, player, isMax, pmax, n_moves,
           node->board.j1_score, node->board.j2_score);

    if (pmax == 0 ||
        check_winning_position(&node->board, player) ||
        check_loosing_position(&node->board, player) ||
        check_draw_position(&node->board)) {
        node->value = h(&node->board, player);
        printf("[build] terminal node=%d h=%d\n", node->node_id, node->value);
        node->nChildren = 0;
        node->children = NULL;
        return node;
    }

    node->nChildren = n_moves;
    if (n_moves > 0) node->children = malloc(n_moves * sizeof(Noeud*));
    else node->children = NULL;

    for (int i = 0; i < n_moves; ++i) {
        Board child_board = copy_board(&node->board);

        make_move(&child_board, moves[i].hole_index, moves[i].type);

        printf("[build] node=%d apply move idx=%d hole=%d type=%d -> child j1=%d j2=%d\n",
               node->node_id, i, moves[i].hole_index, moves[i].type,
               child_board.j1_score, child_board.j2_score);

        node->children[i] = build_tree(&child_board, 1 - player, !isMax, pmax - 1, node_counter);
        node->children[i]->move = moves[i];
    }

    return node;
}

void free_tree(Noeud* node) {
    if (!node) return;
    for (int i = 0; i < node->nChildren; i++) {
        free_tree(node->children[i]);
    }
    free(node->children);
    free(node);
}


int MinMaxValueTree(Noeud* node, int player) {
    if (node == NULL) return 0;

    if (node->nChildren == 0 ||
        check_winning_position(&node->board, player) ||
        check_loosing_position(&node->board, player) ||
        check_draw_position(&node->board)) {
        node->value = h(&node->board, player);
        return node->value;
    }

    if (node->isMax) {
        int best = -VAL_MAX;
        for (int i = 0; i < node->nChildren; ++i) {
            int val = MinMaxValueTree(node->children[i], 1 - player);
            if (val > best) best = val;
        }
        node->value = best;
    } else {
        int best = VAL_MAX;
        for (int i = 0; i < node->nChildren; ++i) {
            int val = MinMaxValueTree(node->children[i], 1 - player);
            if (val < best) best = val;
        }
        node->value = best;
    }
    return node->value;
}

Move decisionMinMaxTree(Noeud* root) {
    Move null_move = {.hole_index = -1, .type = 0};
    if (!root || root->nChildren == 0) return null_move;

    Move bestMove = root->children[0]->move;
    int bestVal = root->children[0]->value;

    for (int i = 1; i < root->nChildren; ++i) {
        int v = root->children[i]->value;
        if ((root->isMax && v > bestVal) || (!root->isMax && v < bestVal)) {
            bestVal = v;
            bestMove = root->children[i]->move;
        }
    }
    return bestMove;
}

int AlphaBetaValueTree(Noeud* node, int player, int alpha, int beta) {
    if (node == NULL) return 0;

    if (node->nChildren == 0 ||
        check_winning_position(&node->board, player) ||
        check_loosing_position(&node->board, player) ||
        check_draw_position(&node->board)) {
        node->value = h(&node->board, player);
        return node->value;
    }

    if (node->isMax) {
        int value = -VAL_MAX;
        for (int i = 0; i < node->nChildren; ++i) {
            int childVal = AlphaBetaValueTree(node->children[i], 1 - player, alpha, beta);
            if (childVal > value) value = childVal;
            if (value > alpha) alpha = value;
            if (alpha >= beta) { // coupure beta
                break;
            }
        }
        node->value = value;
        return value;
    } else {
        int value = VAL_MAX;
        for (int i = 0; i < node->nChildren; ++i) {
            int childVal = AlphaBetaValueTree(node->children[i], 1 - player, alpha, beta);
            if (childVal < value) value = childVal;
            if (value < beta) beta = value;
            if (alpha >= beta) { // coupure alpha
                break;
            }
        }
        node->value = value;
        return value;
    }
}

Move DecisionAlphaBetaTree(Noeud* root, int player) {
    Move null_move = {.hole_index = -1, .type = 0};
    if (!root || root->nChildren == 0) return null_move;

    int alpha = -VAL_MAX, beta = VAL_MAX;
    AlphaBetaValueTree(root, player, alpha, beta);

    return decisionMinMaxTree(root);
}
