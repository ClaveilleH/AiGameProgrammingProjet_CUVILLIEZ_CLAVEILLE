#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "data.h"
#include "game.h"
#include "bot.h"
#include "logger.h"
#include "evaluate.h"
#include "genererDot.h"
#include "arbre.h"


// Board board;
// int PLAYER = 0;


int main() {
    Board board;
    memset(&board, 0, sizeof(Board));
    init_board(&board); 
    // vider juste quelques trous
    int h;

    h = 0;  board.holes[h].R = board.holes[h].B = board.holes[h].T = 0;
    h = 1;  board.holes[h].R = board.holes[h].B = board.holes[h].T = 0;
    h = 2;  board.holes[h].R = board.holes[h].B = board.holes[h].T = 0;
    h = 3;  board.holes[h].R = board.holes[h].B = board.holes[h].T = 0;
    h = 4;  board.holes[h].R = board.holes[h].B = board.holes[h].T = 0;
    h = 5;  board.holes[h].R = board.holes[h].B = board.holes[h].T = 0;
    h = 6; board.holes[h].R = board.holes[h].B = board.holes[h].T = 0;
    h = 7;  board.holes[h].R = board.holes[h].B = board.holes[h].T = 0;
    h = 8;  board.holes[h].R = board.holes[h].B = board.holes[h].T = 0;
    h = 9;  board.holes[h].R = board.holes[h].B = board.holes[h].T = 0;
    h = 10;  board.holes[h].R = board.holes[h].B = board.holes[h].T = 0;
    h = 11;  board.holes[h].R = board.holes[h].B = board.holes[h].T = 0;
    h = 12;  board.holes[h].R = board.holes[h].B = board.holes[h].T = 0;
    h = 13;  board.holes[h].R = board.holes[h].B = board.holes[h].T = 0;
    // h = 14;  board.holes[h].R = board.holes[h].B = board.holes[h].T = 0;
    // h = 15;  board.holes[h].R = board.holes[h].B = board.holes[h].T = 0;
  
    printf("INIT j1=%d j2=%d\n", board.j1_score, board.j2_score);

    Move m = {.hole_index=0, .type=R};
    Board nb = copy_board(&board);
    make_move(&nb, m.hole_index, m.type);
    printf("AFTER move j1=%d j2=%d\n", nb.j1_score, nb.j2_score);

    int node_counter = 0;
    int pmax = 3;
    int player = 0;

    // Construire l'arbre
    Noeud* arbre = build_tree(&board, player, 1, pmax, &node_counter);
    
    // Calculer les valeurs MinMax
    MinMaxValueTree(arbre, player);
    
    // Générer le fichier DOT
    ouvrir_graphe();
    generer_dot_arbre(arbre);
    fermer_graphe();

    printf("Graphe DOT généré dans graphe.dot\n");

    return 0;
}


