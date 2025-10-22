#include "data.h"
#include "game.h"

void play_game(Board* board) {
    // Game loop and logic here
}

void make_move(Board* board, int hole_index, SeedType type) {
    // Move logic here
    /*
    Differents moves:
        [0..15] R
                B
                TR
                TB 
    */


    Hole* hole = get_hole(board, hole_index);
    switch (type) {
        case R:
            /* code for red */
            for (int i = 0; i < hole->R; i++) {
                // Example logic: distribute red seeds
                board->holes[(hole_index + i + 1) % MAX_HOLES].R += 1;
            }
            hole->R = 0;
            break;
        case B:
            /* code for blue */
            for (int i = 0; i < hole->B; i++) {
                // Example logic: distribute blue seeds
                board->holes[(hole_index + (i * 2) + 1) % MAX_HOLES].B += 1;
            }
            hole->B = 0;
            break;
        case TR:
            /* code for transparent red */
            break;
        case TB:
            /* code for transparent blue */
            break;
        default:
            fprintf(stderr, "Invalid seed type\n");
            exit(EXIT_FAILURE);
            break;
    }
    int captured = 0;
    if (test_capture(board, hole_index, &captured)) {
        printf("Captured %d seeds!\n", captured);
        //! mettre a jour le score et le nombre de graines du board
    }

}

static inline int get_previous_hole_index(int current_index) {
    return (current_index - 1 + MAX_HOLES) % MAX_HOLES;
}

int test_capture(Board* board, int hole_index, int *captured) {
    // Capture testing logic here
    Hole* hole = get_hole(board, hole_index);
    int total_captured = 0;
    
    int nb_graines = hole->R + hole->B + hole->T;
    // Example logic: capture if there are more than 3 seeds of any color
    if (nb_graines == 3 || nb_graines == 2) {
        test_capture(board, get_previous_hole_index(hole_index), &total_captured);
        total_captured += hole->R;
        hole->R = 0;

    }

    *captured = total_captured;
    return total_captured > 0;
}

int check_winner(const Board* board, int *winner) {
    // Winner checking logic here
    if (board->j1_score >= 49) {
        *winner = 1;
        return 1;
    } else if (board->j2_score >= 49) {
        *winner = 2;
        return 1;
    }
    return 0;
}
