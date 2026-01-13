# AiGameProgrammingProjet

Projet réalisé dans le cadre de l’UE **AI Game Programming** (Master 1 Informatique).

Il s’agit d’une intelligence artificielle en **C++** capable de jouer à une variante du jeu de l’Oware, en utilisant un algorithmes de recherche Alpha-Bêta et des heuristiques.
---

## Règles du jeu

- Plateau de **16 trous**, numérotés de 1 à 16.
- Le joueur 1 contrôle les trous impairs, le joueur 2 les trous pairs.
- Chaque trou contient initialement :
  - 2 graines rouges
  - 2 graines bleues
  - 2 graines transparentes  
  → soit **96 graines au total**.

### Objectif
Capturer plus de graines que l’adversaire (49 pour une victoire directe).

### Couleurs de graines
- **Rouges** : semées dans tous les trous.
- **Bleues** : semées uniquement dans les trous adverses.
- **Transparentes** : jouées comme rouges ou bleues au choix, mais restent transparentes.

### Fin de partie
- Un joueur atteint **49 graines ou plus**
- Ou il reste **moins de 10 graines sur le plateau**
- Ou affamement d'un joueur
---

###Intelligence artificielle

### Choix techniques
- **Langage** : C++
- Raisons :
  - performances élevées
  - contrôle de la mémoire
  - adapté aux algorithmes récursifs profonds

### Algorithmes utilisés
- **Alpha-Bêta pruning**
- **Itérative Deepening** pour exploiter pleinement la limite de temps (2 secondes par coup)

### Heuristiques principales
- Différence de score
- Nombre de coups légaux
- Graines capturables à court terme
- Conservation des graines transparentes
- Affamement de l’adversaire
- Pénalisation des trous vulnérables
- Réduction des options adverses

Une part d’aléatoire est introduite sur les premiers coups afin d’éviter des parties trop déterministes.

---

## Contraintes de compétition

- **400 coups maximum par partie**
- **2 secondes par coup**

---
## Exécution du projet pour la compétition

Le code est compilé avec **gcc** sous Linux et génère l’exécutable :
- `aigame` (Linux)
- `aigame.exe` (Windows)

```
gcc main.c data.c game.c bot.c logger.c  evaluate.c  -o aigame  
```
La compétition peut être lancée à l’aide de l’arbitre Java fourni :

```
javac Arbitre.java
java Arbitre
```
---
## Exécution du projet

Comment executer ? 

Plusieurs modes sont disponibles :
- debug : mode de debug avec logs
- compete : mode compétition sans logs de debug

Comment exécuter ?
1. Compiler le projet en mode 'debug' :
   ```
   make MODE=debug
   ```
   On peut rajouter 'clean' pour nettoyer les anciens fichiers compilés :
   ```
   make clean MODE=debug
   ```
   Ou 'rebuild' pour nettoyer et recompiler :
   ```
   make rebuild MODE=debug
   ```
   Ou 'run' pour exécuter directement après compilation :
   ```
   make run MODE=debug
   ```

2. Compiler le projet en mode 'compete' :
   ```
   make MODE=compete
   ```
   Pour exécuter la "compétition" entre deux agents :
   ```
   python3 compete.py
   ```
