#ifndef BOT_H
#define BOT_H

#include "data.h"
#include "game.h"



void bot_play(Board* board);
int get_sorted_move_list(Board* board, Move* move_list, int player, Move previousBestMove);
int get_move_list(Board* board, Move* move_list, int player);
Board* simulate_move(Board* board, Move move);
int calculate_max_depth(Board* board, double eval_time, double dispo_time);

#endif // BOT_H