#ifndef EVALUATE_H
#define EVALUATE_H

#include "data.h"


int check_winning_position(Board* board, int player);
int check_loosing_position(Board* board, int player);
int check_draw_position(Board* board);
int h(Board* board, int player);
int h1(Board* board, int player);
int h2(Board* board, int player);
int h3(Board* board, int player);
int h4(Board* board, int player);
int h5(Board* board, int player);
int evaluate(Board* board, int player);

int h(Board* board, int player);
Move decisionMinMax ( Board* board, int player, int pmax );
int minMaxValue (Board* board, int player, int isMax, int pmax);
Move decisionAlphaBeta ( Board* board, int player, int pmax );
int alphaBetaValue (Board* board, int player, int alpha, int beta, int isMax, int pmax, MoveList* moveList);
Move iterative_deepening_search(Board* board, int player, double time_limit_ms, int max_depth);

#endif // EVALUATE_H
