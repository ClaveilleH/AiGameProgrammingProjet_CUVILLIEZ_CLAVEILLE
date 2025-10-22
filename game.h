#ifndef GAME_H
#define GAME_H

#include "data.h"

void play_game(Board* board);
void make_move(Board* board, int hole_index, SeedType type);
int test_capture(Board* board, int hole_index, int *captured);
int check_winner(const Board* board, int *winner);

#endif // GAME_H