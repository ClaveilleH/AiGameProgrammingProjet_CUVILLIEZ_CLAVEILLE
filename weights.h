#ifndef WEIGHTS_H
#define WEIGHTS_H

// #define TOTAL_SEED_W 1 
// #define TOTAL_SEED_OPP_W 1
// #define TOTAL_SEED_PLAYER_W 1 
// #define SCORE_DIF_W 10
// #define W1 0.198649 
// #define W2 0.190084 
// #define W3 0.370793 
// #define W4 1
// #define W5 0.565937

// #define H1_W 19
// #define H2_W 19
// #define H3_W 37
// #define H4_W 100
// #define H5_W 41
// #define H6_W 56

typedef struct {
    double H1_W, H2_W, H3_W, H4_W, H5_W, H6_W, H7_W, H8_W, H9_W, H10_W, H11_W, H12_W;
    double W1, W2, W3, W4, W5;
    double SCORE_DIF_W;
    double TOTAL_SEED_PLAYER_W;
    double TOTAL_SEED_OPP_W;
    double TOTAL_SEED_W;
} Weights;

extern Weights weights;

void load_weights_from_file(const char *path);

#endif // WEIGHTS_H