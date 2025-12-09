#ifndef GENERER_DOT_H
#define GENERER_DOT_H

#include "data.h"
#include "arbre.h"


// Génère un graphe DOT à partir d'un seul nœud (AST)
int generer_dot_node(int bestVal);

// Fonctions de gestion du fichier DOT
void ouvrir_graphe();
void fermer_graphe();

// Création de nœuds dans le graphe
int noeudPersonnalisable(const char* label, const char* shape, const char* color, const char* style);

// Création d'une arête entre deux nœuds
void fleche(int from, int to); // renommée pour rester cohérent avec genererDot.c
void fleche_move(int from, int to, Move move);

void generer_dot_arbre(Noeud* node);

#endif