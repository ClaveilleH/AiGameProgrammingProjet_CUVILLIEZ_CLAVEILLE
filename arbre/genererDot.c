#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> 


#include "data.h"
#include "game.h"
#include "bot.h"
#include "evaluate.h"
#include "genererDot.h"
#include "arbre.h"
#include "logger.h"


static int compteur = 0;
static FILE *f_dot = NULL;


// Variable globale pour compter les cases
int compteurCase;

void ouvrir_graphe() {
    f_dot = fopen("graphe.dot", "w");
    if (!f_dot) {
        perror("Erreur d'ouverture du fichier graphe.dot");
        return;
    }
    fprintf(f_dot, "digraph G {\n");
}

void fermer_graphe() {
    if (f_dot) {
        fprintf(f_dot, "}\n");
        fclose(f_dot);
        f_dot = NULL;
    }
}

void fleche(int from, int to) {
    fprintf(f_dot, "node%d -> node%d;\n", from, to);
}

void fleche_move(int from, int to, Move move) {
    fprintf(f_dot, "node%d -> node%d [label=\"h=%d, t=%d\"];\n", from, to, move.hole_index, move.type);
}


int noeudPersonnalisable(const char* label, const char* shape, const char* color, const char* style) {
    int id = compteur++;
    fprintf(f_dot, "node%d [label=\"%s\"", id, label);
    if (shape) fprintf(f_dot, " shape=%s", shape);
    if (color) fprintf(f_dot, " color=%s", color);
    if (style) fprintf(f_dot, " style=%s", style);
    fprintf(f_dot, "];\n");
    return id;
}

int generer_dot_node(int bestVal) {
    int id;
    char label[32];
    snprintf(label, sizeof(label), "%d", bestVal);
    id = noeudPersonnalisable(label, "ellipse", "black", NULL);
    return id;


}

void generer_dot_arbre(Noeud* node) {
    if (!node) return;

    // Créer un noeud pour DOT avec la valeur heuristique
    int node_id = noeudPersonnalisable(node->nChildren == 0 ? 
    (node->move.hole_index >= 0 ? (char[32]){0} : (char[32]){0}) 
            : (char[32]){0}, 
        "ellipse", node->isMax ? "lightblue" : "pink", "filled"
    );

    char label[64];
    if (node->move.hole_index >= 0) {
        snprintf(label, sizeof(label), "v=%d", node->value);
    } else {
        snprintf(label, sizeof(label), "v=%d", node->value);
    }
    fprintf(f_dot, "node%d [label=\"%s\", shape=ellipse, style=filled, color=%s];\n",
            node_id, label, node->isMax ? "lightblue" : "pink");

    // Parcours récursif des enfants
    for (int i = 0; i < node->nChildren; i++) {
        generer_dot_arbre(node->children[i]);
        fleche_move(node_id, node->children[i]->node_id, node->children[i]->move);
    }

    node->node_id = node_id; // on stocke l'ID DOT pour les flèches
}

void generer_dot_arbre_v2(Noeud* node) {
    if (!node) return;

    // Création du label affiché dans le nœud
    char label[64];

    if (node->move.hole_index >= 0)
        snprintf(label, sizeof(label), "v=%d\n(h=%d,t=%d)", 
                 node->value, node->move.hole_index, node->move.type);
    else
        snprintf(label, sizeof(label), "v=%d\n(root)", node->value);

    // Création du noeud DOT et récupération de son ID
    int node_id = noeudPersonnalisable(
        label,
        "ellipse",
        node->isMax ? "lightblue" : "pink",
        "filled"
    );

    // Mémorisation de l’ID DOT
    node->node_id = node_id;

    // Dessiner les enfants
    for (int i = 0; i < node->nChildren; i++) {
        Noeud* child = node->children[i];

        // Récursion : génère les enfants d'abord
        generer_dot_arbre(child);

        // Puis relie le noeud courant à l'enfant
        fleche_move(node_id, child->node_id, child->move);
    }
}
