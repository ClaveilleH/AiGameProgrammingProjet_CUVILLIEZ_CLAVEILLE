#ifndef ARBRE_H
#define ARBRE_H

#include "data.h"

typedef struct Noeud {
    int node_id;
    int value; 
    Board board;
    Move move;
    int player;
    int isMax;
    struct Noeud** children; 
    int nChildren;
} Noeud;

// Fonctions de l'arbre MinMax
Noeud* create_noeud(int node_id, Board board, Move move, int player, int isMax);
Noeud* build_tree(const Board* board, int player, int isMax, int pmax, int* node_counter);
void free_tree(Noeud* node);

int MinMaxValueTree(Noeud* node, int player);
Move decisionMinMaxTree(Noeud* root);

int AlphaBetaValueTree(Noeud* node, int player, int alpha, int beta);
Move DecisionAlphaBetaTree(Noeud* root, int player);
Board copy_board(const Board* board);

#endif // DATA_H
