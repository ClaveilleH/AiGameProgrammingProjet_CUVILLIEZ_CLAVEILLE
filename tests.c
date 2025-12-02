#include <stdio.h>
#include <stdlib.h>


#include "data.h"
#include "bot.h"
#include "game.h"
// #include "bot.h"
#include "evaluate.h"
#include "weights.h"
// #include "logger.h"


void _print_board(const Board* board) {
    printf("Score - Player 1: %d, Player 2: %d\n", board->j1_score, board->j2_score);
    printf("Board state:\n");
    for (int i = 0; i < 16; i++) {
        printf("Hole ");
        if (i < 10) {
            printf(" ");
        }
        printf("%d: R=", i);
        if (board->holes[i].R < 10) {
            printf(" ");
        }
        printf("%d, B=", board->holes[i].R);
        if (board->holes[i].B < 10) {
            printf(" ");
        }
        printf("%d, T=", board->holes[i].B);
        if (board->holes[i].T < 10) {
            printf(" ");
        }
        printf("%d\n", board->holes[i].T);
        // printf("Hole %d: R=%d, B=%d, T=%d\n", i, board->holes[i].R, board->holes[i].B, board->holes[i].T);
    }
    printf("Total seed count: %d\n", board->seed_count);
}

void print_move(Move move) {
    const char* type_str;
    
    switch(move.type) {
        case R:  type_str = "R"; break;
        case B:  type_str = "B"; break;
        case TR: type_str = "TR"; break;
        case TB: type_str = "TB"; break;
        default: type_str = "UNKNOWN"; break;
    }
    
    printf("Move: hole %d, type %s\n", move.hole_index, type_str);
    fflush(stdout);  // Force l'affichage immédiat
}


void testH1(void) {
    // Placeholder for test 1
    printf("Running Test 1...\n");
    Board board;
    init_board(&board);
    printf("DEBUG value : %d\n", DEBUG);
    _print_board(&board);
    int value = h(&board, 0);
    printf("Heuristic value for player 0: %d\n", value);
    value = h(&board, 1);
    printf("Heuristic value for player 1: %d\n", value);
}

void testH2(void) {
    // Placeholder for test 2
    printf("Running Test 2...\n");
    Board board;
    init_board(&board);
    // Modify board state for testing
    board.j1_score = 30;
    board.j2_score = 20;
    board.holes[0].R = 5;
    board.holes[1].B = 3;
    _print_board(&board);
    int value = h(&board, 0);
    printf("Heuristic value for player 0: %d\n", value);
    value = h(&board, 1);
    printf("Heuristic value for player 1: %d\n", value);
}

void testH3(void) {
    // Placeholder for test 3
    printf("Running Test 3...\n");
    Board board;
    init_board(&board);
    make_move(&board, 0, TR);
    _print_board(&board);
    int value = h(&board, 0);
    printf("Heuristic value for player 0: %d\n", value);
    value = h(&board, 1);
    printf("Heuristic value for player 1: %d\n", value);
    Move bestMove = decisionMinMax(&board, 0, 3);
    // Move bestMove = decisionAlphaBeta(&board, 0, 3);
    printf("Best move for player 0: hole %d, type %s\n", bestMove.hole_index, 
        (bestMove.type == R) ? "R" : 
        (bestMove.type == B) ? "B" : 
        (bestMove.type == TR) ? "TR" : "TB");
    // print_move(bestMove);
    bestMove = decisionMinMax(&board, 1, 3);
    // bestMove = decisionAlphaBeta(&board, 1, 3);
    printf("Best move for player 1: hole %d, type %s\n", bestMove.hole_index, 
        (bestMove.type == R) ? "R" : 
        (bestMove.type == B) ? "B" : 
        (bestMove.type == TR) ? "TR" : "TB");
    make_move(&board, bestMove.hole_index, bestMove.type);
    _print_board(&board);
}


void testH4() {
    Board board;
    init_board(&board); 
    // vider juste quelques trous
    int c;
    c = 0;  board.holes[c].R = board.holes[c].B = board.holes[c].T = 0;
    c = 1;  board.holes[c].R = board.holes[c].B = board.holes[c].T = 0;
    //c = 2;  board.holes[c].R = board.holes[c].B = board.holes[c].T = 0;
    c = 3;  board.holes[c].R = board.holes[c].B = board.holes[c].T = 0;
    c = 4;  board.holes[c].R = board.holes[c].B = 0;
    c = 5;  board.holes[c].R = board.holes[c].B = 0;
    c = 5;  board.holes[c].T = 2;
    c = 6;  board.holes[c].R = board.holes[c].B = 0;
    c = 6;  board.holes[c].T = 2;
    c = 7;  board.holes[c].R  = board.holes[c].T = 0;
    c = 7;  board.holes[c].B = 2;
    c = 8;  board.holes[c].R = board.holes[c].B = board.holes[c].T = 0;
    c = 9;  board.holes[c].R = board.holes[c].B = board.holes[c].T = 0;
    c = 10;  board.holes[c].R = board.holes[c].B = board.holes[c].T = 0;
    c = 11;  board.holes[c].R = board.holes[c].B = board.holes[c].T = 0;
    c = 12;  board.holes[c].R = board.holes[c].B = board.holes[c].T = 0;
    c = 13;  board.holes[c].R = board.holes[c].B = board.holes[c].T = 0;
    c = 14;  board.holes[c].R = board.holes[c].B = board.holes[c].T = 0;
    c = 15;  board.holes[c].R = board.holes[c].B = board.holes[c].T = 0;
   
  
    make_move(&board, 0, TR);
    _print_board(&board);
    int value = evaluate(&board, 0);
    printf("Heuristic value for player 0: %d\n", value);
    value = evaluate(&board, 1);
    printf("Heuristic value for player 1: %d\n", value);
    Move bestMove = decisionMinMax(&board, 1, 3);
    // Move bestMove = decisionAlphaBeta(&board, 0, 3);
    printf("Best move for player 1: hole %d, type %s\n", bestMove.hole_index, 
        (bestMove.type == R) ? "R" : 
        (bestMove.type == B) ? "B" : 
        (bestMove.type == TR) ? "TR" : "TB");
    // print_move(bestMove);
    bestMove = decisionMinMax(&board, 0, 3);
    printf("Best move for player 0 minimax: hole %d, type %s\n", bestMove.hole_index, 
        (bestMove.type == R) ? "R" : 
        (bestMove.type == B) ? "B" : 
        (bestMove.type == TR) ? "TR" : "TB");
    bestMove = decisionAlphaBeta(&board, 0, 3);
    printf("Best move for player 0 alpha beta: hole %d, type %s\n", bestMove.hole_index, 
        (bestMove.type == R) ? "R" : 
        (bestMove.type == B) ? "B" : 
        (bestMove.type == TR) ? "TR" : "TB");
    make_move(&board, bestMove.hole_index, bestMove.type);
    _print_board(&board);

}


void test_move_list(void) {
    Board board;
    init_board(&board);
    board.holes[0].R = 0;
    board.holes[0].B = 0;
    // board.holes[0].R = 3;
    // board.holes[1].B = 2;
    // board.holes[2].T = 1;
    _print_board(&board);
    Move moves[MAX_HOLES/2*4];
    int n_moves = get_move_list(&board, moves, 0);
    printf("Possible moves for player 0:\n");
    for (int i = 0; i < n_moves; i++) {
        print_move(moves[i]);
    }
    bot_play(&board);
}

void testH4(){
    Board board;
    init_board(&board);
    make_move(&board, 0, TR);
    printf("value h player 0: %d\n", h(&board,0));
    // printf("value h player 1: %d\n", h(&board,1));
    make_move(&board, 13, B);
    printf("value h player 0 after move: %d\n", h(&board,0));
    printf("value h player 1 after move: %d\n", h(&board,1));
    _print_board(&board);
}

int main(int argc, char* argv[]) {
    //testH1();
    //printf("----------------------------------------------------\n");
    //testH2();
    //printf("----------------------------------------------------\n");
    //testH3();
    testH4();
    //test_move_list();
    return 0;
}