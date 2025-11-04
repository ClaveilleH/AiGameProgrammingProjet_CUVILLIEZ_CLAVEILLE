#include "data.h"
#include "game.h"
#include "bot.h"
#include "logger.h"

#include <stdlib.h>
#include <time.h>


void bot_play(Board* board) {
    // Simple bot logic: choose random move till a valid one is found
    int hole_index;
    SeedType type;
    int valid_move = 0;
    while (!valid_move) {
        hole_index = rand() % MAX_HOLES/2 + PLAYER;
        int type_choice = rand() % 4;
        switch (type_choice) {
            case 0:
                type = R;
                break;
            case 1:
                type = B;
                break;
            case 2:
                type = TR;
                break;
            case 3:
                type = TB;
                break;
            default:
                type = R; // Pour eviter un warning, mais ne devrait jamais arriver
                break;
        }
        if (is_valid_move(board, hole_index, type)) {
            valid_move = 1;
        }
    }
    if (COMPETE) {
        printf("%d %s\n", hole_index + 1, 
            (type == R) ? "R" : 
            (type == B) ? "B" : 
            (type == TR) ? "TR" : "TB");
    } else {
        COMPETE_PRINT("Bot chooses hole %d with seed type %d\n", hole_index, type);
    }
    log("Bot chooses hole %d with seed type %s", hole_index, (type == R) ? "R" : 
        (type == B) ? "B" : 
        (type == TR) ? "TR" : "TB");
    make_move(board, hole_index, type);
    
}