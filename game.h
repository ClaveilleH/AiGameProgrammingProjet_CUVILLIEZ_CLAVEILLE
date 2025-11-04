#ifndef GAME_H
#define GAME_H

#include "data.h"

void play_game(Board* board);
int make_move(Board* board, int hole_index, SeedType type);
int test_capture(Board* board, int hole_index, int *captured);
int check_winner(const Board* board, int *winner);
int is_valid_move(Board* board, int hole_index, SeedType type, int playerId);

// ! DETECT ILLEGAL MOVES ?
int detect_illegal_moves(Board* board);

#endif // GAME_H