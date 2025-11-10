#include "data.h"
#include "game.h"
#include "bot.h"
#include "logger.h"


#include <stdlib.h>
#include <time.h>

int get_move_list(Board* board, Move* move_list) { 
 
    int index = 0;
    printf("Possible moves : \n"); 
    // Function to get possible moves
    for (int i = PLAYER ; i < MAX_HOLES; i+=2) { 
        // Check each hole for possible moves 
        Hole* hole = get_hole(board, i); 
        int nb_graines_R = hole->R; 
        int nb_graines_B = hole->B; 
        int nb_graines_T = hole->T; 
        if(nb_graines_B>0) {
            move_list[index++] = (Move){i, B};
            printf("Move B, hole %d\n", i ); 
        }
        if(nb_graines_R>0) {
            move_list[index++] = (Move){i, R};
            printf("Move R, hole %d\n", i ); 
        }
        if(nb_graines_T>0) { 
            move_list[index++] =  (Move){i, TB};
            printf("Move TB, hole %d\n", i ); 
            move_list[index++] =  (Move){i, TR};
            printf("Move TR, hole %d\n", i ); } 
        } 
    printf("Nb total de moves : %d\n", index);
    return index;
    }

Board* simulate_move(Board* board, Move move){
    //On simule le plateau de jeu selon le coup choisi
    Board* copie_board = dup_board(board);
    if (!copie_board) {
        fprintf(stderr, "simulate_move: erreur allocation mémoire\n");
        exit(EXIT_FAILURE);
    }
    make_move(copie_board, move.hole_index, move.type);
    return copie_board;

}

void bot_play(Board* board) {
    // Simple bot logic: choose random move till a valid one is found
    int hole_index;
    SeedType type;
    int valid_move = 0;
    while (!valid_move) {
        hole_index = (rand() % (MAX_HOLES / 2)) * 2 + (1 - PLAYER);
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
        if (is_valid_move(board, hole_index, type, PLAYER)) {
            valid_move = 1;
        }
    }
    if (COMPETE) {
        printf("%d %s\n", hole_index + 1, 
            (type == R) ? "R" : 
            (type == B) ? "B" : 
            (type == TR) ? "TR" : "TB");
    } else {
        COMPETE_PRINT("Bot(%d) chooses hole %d with seed type %d\n", PLAYER, hole_index, type);

        printf(">>%d %s\n", hole_index + 1, 
            (type == R) ? "R" : 
            (type == B) ? "B" : 
            (type == TR) ? "TR" : "TB");
    }
    log("Bot chooses hole %d with seed type %s", hole_index, (type == R) ? "R" : 
        (type == B) ? "B" : 
        (type == TR) ? "TR" : "TB");
    make_move(board, hole_index, type);
    
}