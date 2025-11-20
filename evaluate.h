#ifndef EVALUATE_H
#define EVALUATE_H

#include "data.h"



int check_winning_position(Board* board, int player);
int check_losing_position(Board* board, int player);
int check_draw_position(Board* board, int player);

int h(Board* board, int player);
Move decisionMinMax ( Board* board, int player, int pmax );
int minMaxValue (Board* board, int player, int isMax, int pmax);
Move decisionAlphaBeta ( Board* board, int player, int pmax );
int alphaBetaValue (Board* board, int player, int alpha, int beta, int isMax, int pmax);

#endif // EVALUATE_H
