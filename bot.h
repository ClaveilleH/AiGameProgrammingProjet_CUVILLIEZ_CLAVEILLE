#ifndef BOT_H
#define BOT_H

#include "data.h"
#include "game.h"

extern int coups;
void bot_play(Board* board);
int get_move_list(Board* board, Move* move_list, int player);
Board* simulate_move(Board* board, Move move);
int compute_depth(Board* board);

#endif // BOT_H