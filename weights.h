#ifndef WEIGHTS_H
#define WEIGHTS_H

typedef struct {
    double H1_W, H2_W, H3_W, H4_W, H5_W, H6_W;
    double W1, W2, W3, W4, W5;
    double SCORE_DIF_W;
    double TOTAL_SEED_PLAYER_W;
    double TOTAL_SEED_OPP_W;
    double TOTAL_SEED_W;
} Weights;

extern Weights weights;

void load_weights_from_file(const char *path);

#endif
