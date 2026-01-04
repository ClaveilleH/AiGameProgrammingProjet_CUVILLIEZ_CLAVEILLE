#include "weights.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Weights weights = {
    .H1_W = 1.0,
    .H2_W = 1.0,
    .H3_W = 1.0,
    .H4_W = 1.0,
    .H5_W = 1.0,
    .H6_W = 1.0,
    .W1 = 1.0, .W2 = 1.0, .W3 = 1.0, .W4 = 1.0, .W5 = 1.0,
    .SCORE_DIF_W = 1.0,
    .TOTAL_SEED_PLAYER_W = 1.0,
    .TOTAL_SEED_OPP_W = 1.0,
    .TOTAL_SEED_W = 1.0
};

static void parse_line(char *line) {
    char key[128];
    double val;
    if (sscanf(line, " %127[^=]=%lf", key, &val) == 2) {
        if (strcmp(key, "H1_W")==0) weights.H1_W = val;
        else if (strcmp(key, "H2_W")==0) weights.H2_W = val;
        else if (strcmp(key, "H3_W")==0) weights.H3_W = val;
        else if (strcmp(key, "H4_W")==0) weights.H4_W = val;
        else if (strcmp(key, "H5_W")==0) weights.H5_W = val;
        else if (strcmp(key, "H6_W")==0) weights.H6_W = val;
        else if (strcmp(key, "H7_W")==0) weights.H7_W = val;
        else if (strcmp(key, "H8_W")==0) weights.H8_W = val;
        else if (strcmp(key, "H9_W")==0) weights.H9_W = val;
        else if (strcmp(key, "H10_W")==0) weights.H10_W = val;
        else if (strcmp(key, "H11_W")==0) weights.H11_W = val;
        else if (strcmp(key, "H12_W")==0) weights.H12_W = val;
        else if (strcmp(key, "W1")==0) weights.W1 = val;
        else if (strcmp(key, "W2")==0) weights.W2 = val;
        else if (strcmp(key, "W3")==0) weights.W3 = val;
        else if (strcmp(key, "W4")==0) weights.W4 = val;
        else if (strcmp(key, "W5")==0) weights.W5 = val;
        else if (strcmp(key, "SCORE_DIF_W")==0) weights.SCORE_DIF_W = val;
        else if (strcmp(key, "TOTAL_SEED_PLAYER_W")==0) weights.TOTAL_SEED_PLAYER_W = val;
        else if (strcmp(key, "TOTAL_SEED_OPP_W")==0) weights.TOTAL_SEED_OPP_W = val;
        else if (strcmp(key, "TOTAL_SEED_W")==0) weights.TOTAL_SEED_W = val;
        /* add more keys if needed */
    }
}

void load_weights_from_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return; // use defaults if file absent
    char line[256];
    while (fgets(line, sizeof(line), f)) {
        if (line[0] == '#' || line[0] == '\n') continue;
        parse_line(line);
    }
    fclose(f);
}
