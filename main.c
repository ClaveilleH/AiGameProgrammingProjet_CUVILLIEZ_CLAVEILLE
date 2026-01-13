#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>

#include "data.h"
#include "game.h"
#include "bot.h"
#include "logger.h"
#include <time.h>
// #include <sys/time.h>

Board board;
int run;

int get_human_move(int *hole_index, SeedType *type) {
    /*
    Lit et parse le coup de l'autre joueur depuis l'entrée standard.
    q pour quitter
    */
    COMPETE_PRINT("Enter your move (hole index and seed type): ");
    int idx;
    char type_str[10]; // Buffer pour lire la chaîne complète

    if (scanf("%d%s", &idx, type_str) != 2) {
        if (scanf("%s", type_str)) {;}
        if (strcmp(type_str, "q") == 0) {
            fprintf(stderr, "Player chose to quit the game.\n");
            run = 0;
            return 1;
        }
        fprintf(stderr, "(%d)Invalid input format. got %d, %s\n", PLAYER, idx, type_str);
        // Nettoyer le buffer d'entrée
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        return 1;
    }

    if (idx < 0 || idx > MAX_HOLES) {
        fprintf(stderr, "Invalid hole index. Must be between 0 and %d.\n", MAX_HOLES - 1);
        return 1;
    }

    SeedType seed_type;
    if (strcmp(type_str, "R") == 0) {
        seed_type = R;
    } else if (strcmp(type_str, "B") == 0) {
        seed_type = B;
    } else if (strcmp(type_str, "TR") == 0) {
        seed_type = TR;
    } else if (strcmp(type_str, "TB") == 0) {
        seed_type = TB;
    } else {
        fprintf(stderr, "Invalid seed type. Use 'R', 'B', 'TR', or 'TB' entered %s.\n", type_str);
        return 1;
    }
    _log("Other player chose hole %d with seed type %s", idx,
        (seed_type == R) ? "R" : 
        (seed_type == B) ? "B" : 
        (seed_type == TR) ? "TR" : "TB");

    *hole_index = idx - 1; // Convertir en index 0-based
    *type = seed_type;
    return 0;
}

int sim_end_game(Board* board) {
    /*
    Transforme le plateau pour simuler une fin de partie.
    */
    for (int i = 0; i < MAX_HOLES; i++) {
        board->holes[i].R = 0;
        board->holes[i].B = 0;
        board->holes[i].T = 0;
    }
    board->seed_count = 15;
    board->j1_score = 45;
    board->j2_score = 46;
    board->holes[0].R = 2;
    board->holes[1].B = 1;
    board->holes[1].T = 3;
    board->holes[2].T = 2;
    board->holes[10].R = 1;
    board->holes[13].B = 1;
    return 0;
}

int get_opponent_move(int *hole_index, SeedType *type) {
    int idx;
    char type_str[8];

    if (scanf("%d %7s", &idx, type_str) != 2)
        return 1;

    *hole_index = idx - 1;

    if (strcmp(type_str, "R") == 0) *type = R;
    else if (strcmp(type_str, "B") == 0) *type = B;
    else if (strcmp(type_str, "TR") == 0) *type = TR;
    else if (strcmp(type_str, "TB") == 0) *type = TB;
    else return 1;

    return 0;
}

int get_opponent_move2(int *hole_index, SeedType *type) {
    char line[128];
    if (fgets(line, sizeof(line), stdin) == NULL) return 1;

    line[strcspn(line, "\n")] = 0;

    // Si c'est RESULT, on ne tente pas de parser un coup
    if (strncmp(line, "RESULT", 6) == 0) {
        fprintf(stderr, "Game finished: %s\n", line);
        run = 0;
        return 2; // fin de partie
    }

    int idx;
    char type_str[8];
    if (sscanf(line, "%d %7s", &idx, type_str) != 2) return 1;

    *hole_index = idx - 1;
    if (strcmp(type_str, "R") == 0) *type = R;
    else if (strcmp(type_str, "B") == 0) *type = B;
    else if (strcmp(type_str, "TR") == 0) *type = TR;
    else if (strcmp(type_str, "TB") == 0) *type = TB;
    else return 1;

    return 0;
}



int main(int argc, char* argv[]) {
    run = 1;
    int turn = 0; // 0 for player 1, 1 for player 2
    int winner = -1;
    srand(time(NULL));

    init_logger();
    init_board(&board);

    char line[128];

    if (fgets(line, sizeof(line), stdin) == NULL) {
        fprintf(stderr, "No input received, exiting.\n");
        return 1;
    }
     line[strcspn(line, "\n")] = 0; // retirer '\n'

    if (strcmp(line, "START") == 0) {
        PLAYER = 0; // je suis joueur 1
        turn = PLAYER; // mon tour
    } else {
        PLAYER = 1; // je suis joueur 2
        turn = PLAYER;    // joueur 1 commence
        // traiter le premier message comme coup de l’adversaire
        int hole_index;
        SeedType type;
        if (sscanf(line, "%d %7s", &hole_index, line) == 2) {
            if (strcmp(line, "R") == 0) type = R;
            else if (strcmp(line, "B") == 0) type = B;
            else if (strcmp(line, "TR") == 0) type = TR;
            else if (strcmp(line, "TB") == 0) type = TB;
            else type = R; // valeur par défaut
            make_move(&board, hole_index - 1, type, 0); // adversaire = joueur 0
        }
    }

    _log("Game started as Player %d", PLAYER + 1);
    
    //sim_end_game(&board); // For testing end game scenarios
    //print_board(&board);
    while (run) {
    
        if (turn == PLAYER) {
            int end ;
            end = check_end_game(&board, &winner);
            if (end) {
                int coups = board.nb_coups_player1 + board.nb_coups_player2;
                int scoreJ1 = board.j1_score;
                int scoreJ2 = board.j2_score;
                if (coups >= 400){
                    printf("RESULT LIMIT %d %d MAX_TURNS\n", scoreJ1, scoreJ2);
                }
                else{
                    printf("RESULT %d %d %d\n", coups, scoreJ1, scoreJ2);
                }
                fflush(stdout);
                run = 0;
                break;
            }
            DEBUG_PRINT("Bot's turn.\n");
            bot_play(&board);
            // print_board(&board);
        } else {
            DEBUG_PRINT("Player's turn.\n");
            int hole_index;
            SeedType type;

            int ret = get_opponent_move2(&hole_index, &type);
            if (ret == 1) { // coup invalide
                DEBUG_PRINT("Invalid opponent move, skip\n");
                continue;
            } else if (ret == 2) { // RESULT
                break;
            }

            make_move(&board, hole_index, type, 1 - PLAYER);
        }
        
        turn = 1 - turn;
        
    }
    close_logger();
    return 0;
}



/*

make -mode

*/