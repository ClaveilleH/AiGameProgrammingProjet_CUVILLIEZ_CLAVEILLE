#ifndef BOT_H
#define BOT_H

#include "data.h"
#include "game.h"

void bot_play(Board* board);
int get_move_list(Board* board, Move* move_list);
Board* simulate_move(Board* board, Move move);

#endif // BOT_H