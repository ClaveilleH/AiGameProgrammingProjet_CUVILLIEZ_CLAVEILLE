#include "logger.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>

const char* LOG_DIR = "logs/";
FILE* LOGFILE = NULL;

void init_logger() {

    char filename[80];
    char filepath[160];
    struct timeval tv;
    gettimeofday(&tv, NULL);
    time_t now = tv.tv_sec;
    struct tm *t = localtime(&now);

    // Génération du nom : fichier_YYYY-MM-DD_HH-MM-SS_mmm.txt (avec millisecondes)
    int ms = (int)(tv.tv_usec / 1000);
    strftime(filename, sizeof(filename), "fichier_%Y-%m-%d_%H-%M-%S", t);
    /* ajouter les millisecondes et l'extension */
    snprintf(filename + strlen(filename), sizeof(filename) - strlen(filename), "_%03d.txt", ms);
    // fprintf(stderr, "_%03d.txt", ms);
    /* Construire le chemin complet ; LOG_DIR contient un slash final dans la valeur par défaut */
    snprintf(filepath, sizeof(filepath), "%s%s", LOG_DIR, filename);

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
        LOGFILE = NULL;
    }
    

}

void log_print(const char* message) {
    if (LOGFILE) {
        fprintf(LOGFILE, "%s\n", message);
        fflush(LOGFILE);
    }
}
