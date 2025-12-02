#include "data.h"
#include "game.h"
#include "logger.h"

void play_game(Board* board) {
    // Game loop and logic here
    
}

static inline __attribute__((always_inline)) int distribute_red(Board *restrict board, int hole_index, Hole *restrict hole) {
    // Distribute red seeds
    /* __attribute__((always_inline)) pour forcer l'inlining */
    Hole *harr = board->holes;
    int idx = hole_index;
    for (int s = 0, cnt = hole->R; s < cnt; ++s) {
        idx = (idx + 1) & MASK;
        idx = (idx + (idx == hole_index)) & MASK;
        harr[idx].R += 1;
    }
    hole->R = 0;
    return idx; // last sown index (or origin if cnt==0)
}

static inline __attribute__((always_inline)) int distribute_blue(Board *restrict board, int hole_index, Hole *restrict hole) {
    Hole *harr = board->holes;
    int idx = (hole_index + 1) & MASK;
    for (int s = 0, cnt = hole->B; s < cnt; ++s) {
        harr[idx].B += 1;
        idx = (idx + 2) & MASK;
    }
    hole->B = 0;
    // last actually sown is idx - 2 (previous), compute properly:
    idx = (idx - 2 + MAX_HOLES) & MASK;
    // fprintf(stderr, "returned idx: %d\n", (hole->B == 0) ? hole_index : idx);
    // return (hole->B == 0) ? hole_index : idx;
    return idx;
}

static inline __attribute__((always_inline)) int distribute_transparent_red(Board *restrict board, int hole_index, Hole *restrict hole) {
    Hole *harr = board->holes;
    int idx = hole_index;
    for (int s = 0, cnt = hole->T; s < cnt; ++s) {
        idx = (idx + 1) & MASK;
        idx = (idx + (idx == hole_index)) & MASK;
        harr[idx].T += 1;
    }
    hole->T = 0;
    return idx;
}

static inline __attribute__((always_inline)) int distribute_transparent_blue(Board *restrict board, int hole_index, Hole *restrict hole) {
    Hole *harr = board->holes;
    int idx = (hole_index + 1) & MASK;
    for (int s = 0, cnt = hole->T; s < cnt; ++s) {
        harr[idx].T += 1;
        idx = (idx + 2) & MASK;
    }
    idx = (idx - 2 + MAX_HOLES) & MASK;
    return idx;
    // hole->T = 0;
    // return (hole->T == 0) ? hole_index : idx;
}


int make_move(Board* board, int hole_index, SeedType type, int playerId) {
    // Move logic here
    /*
    Differents moves:
        [0..15] R
                B
                TR
                TB 
    */


    Hole* hole = get_hole(board, hole_index);
    int last = hole_index;
    // ! ATTENTION: & MASK pour faire le modulo 16, si MAX_HOLES change, il faut changer le MASK
    
    switch (type) {
        case TR:
            /* code for transparent red */
            last = distribute_transparent_red(board, hole_index, hole);
            // break; /* fallthrough */
            //on enchaine avec les rouge
        case R:
            /* code for red */
            last = distribute_red(board, last, hole);
            break;

        case TB:
            /* code for transparent blue */
            last = distribute_transparent_blue(board, hole_index, hole);
            // break; /* fallthrough */ 
            //on enchaine avec les bleu
        case B:
            /* code for blue */
            last = distribute_blue(board, last, hole);
            break;
            
        default:
            fprintf(stderr, "Invalid seed type\n");
            exit(EXIT_FAILURE);
            break;
    }
    int captured = 0;
    if (test_capture(board, last, &captured)) {
        if (playerId == 0) {
            board->j1_score += captured;
        } else {
            board->j2_score += captured;
        }
        board->seed_count -= captured;
        // printf("Captured %d seeds!\n", captured);
        // COMPETE_PRINT("Captured %d seeds!\n", captured);
        // log("Captured %d seeds!", captured);
        //! mettre a jour le score et le nombre de graines du board
    }
    // fprintf(stderr, "Move completed. Last seed sown at hole %d\n", last);
    // fprintf(stderr, "Total seeds captured this move: %d\n", captured);
    return last;

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
    // printf("Testing capture at hole %d with %d seeds (R=%d, B=%d, T=%d)\n", hole_index, nb_graines, hole->R, hole->B, hole->T);
    if (nb_graines == 3 || nb_graines == 2) {
        test_capture(board, get_previous_hole_index(hole_index), &total_captured);
        total_captured += hole->R;
        hole->R = 0;
        total_captured += hole->B;
        hole->B = 0;
        total_captured += hole->T;
        hole->T = 0;
    }

    *captured = total_captured;

    // if (total_captured > 0) {
    //     log("Total captured seeds: %d", total_captured);
    // }

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

int check_draw(const Board* board) {
    // Draw checking logic here
    if (board->seed_count < 10) {
        return 1; // Draw
    }
    if (board->j1_score >= 40 && board->j2_score >= 40) {
        return 1; // Draw
    }
    return 0; // No draw
}


int is_valid_move(Board* board, int hole_index, SeedType type, int playerId) {
    Hole* hole = get_hole(board, hole_index);
    // Check if the hole belongs to the player
    if ((hole_index % 2) != playerId) {
        fprintf(stderr, "Hole %d does not belong to player %d\n", hole_index, playerId);
        return 0; 
    }
    switch (type) {
        case R:
            if (hole->R == 0) {
                fprintf(stderr, "No red seeds in hole %d\n", hole_index);
            }
            return hole->R != 0;
        case B:
            if (hole->B == 0) {
                fprintf(stderr, "No blue seeds in hole %d\n", hole_index);
            }
            return hole->B != 0;
        case TR:
            if (hole->T == 0) {
                fprintf(stderr, "No transparent seeds in hole %d\n", hole_index);
            }
            return hole->T != 0;
        case TB:
            if (hole->T == 0) {
                fprintf(stderr, "No transparent seeds in hole %d\n", hole_index);
            }
            return hole->T != 0;
        default:
            return 0;
    }
}