# AiGameProgrammingProjet




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
