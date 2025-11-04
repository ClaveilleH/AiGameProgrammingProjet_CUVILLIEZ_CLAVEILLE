#include "logger.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

const char* LOG_DIR = "logs/";
FILE* LOGFILE = NULL;

void init_logger() {

    char filename[64];
    char filepath[128];
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    // Génération du nom : fichier_YYYY-MM-DD_HH-MM-SS.txt
    strftime(filename, sizeof(filename), "fichier_%Y-%m-%d_%H-%M-%S.txt", t);
    snprintf(filepath, sizeof(filepath), "%s/%s", LOG_DIR, filename);

    // Exemple : création du fichier
    LOGFILE = fopen(filepath, "w");
    if (LOGFILE == NULL) {
        perror("Erreur d'ouverture du fichier");
    }

    fprintf(LOGFILE, "Fichier créé à %s", asctime(t));

}

void close_logger() {
    if (LOGFILE) {
        fclose(LOGFILE);
    }
}

void log_print(const char* message) {
    if (LOGFILE) {
        fprintf(LOGFILE, "%s\n", message);
        fflush(LOGFILE);
    }
}
