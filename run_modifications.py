#!/usr/bin/env python3
import subprocess
import csv
import random
import matplotlib.pyplot as plt
import os
import re

# -------------------------------
# Paramètres
# -------------------------------
BINARY = "./aigame"  # exécutable
BASE_WEIGHTS = {
    "H1_W": 0,
    "H2_W": 1,
    "H3_W": 25,
    "H4_W": 700,
    "H5_W": 20,
    "H6_W": 0.0,
    "H7_W": 4,
    "H8_W": 3,
    "H9_W": 21,
    "H10_W": 23,
    "H11_W": 4.34,
    "H12_W": 9.34,
}

N_COMBOS = 20    # nombre de combinaisons aléatoires à tester
N_MATCHES = 2    # nombre de matchs par paire
VARIATION = 1  # ±20% autour du poids de base
TIMEOUT = 120     # secondes max par match

OUT_CSV = "tournament_results.csv"
OUT_PNG = "scores.png"

# -------------------------------
# Fonctions utilitaires
# -------------------------------
def write_weights_file(filename, weights):
    with open(filename, "w") as f:
        for k, v in weights.items():
            f.write(f"{k}={v}\n")

def randomize_weights(base, variation=0.2):
    new_weights = {}
    for k, v in base.items():
        delta = v * variation
        new_weights[k] = max(0, round(v + random.uniform(-delta, delta), 2))
    return new_weights

def play_match(wA_file, wB_file):
    cmd = [BINARY, "autoplay", wA_file, wB_file]
    try:
        proc = subprocess.run(cmd, capture_output=True, text=True, timeout=TIMEOUT)
    except subprocess.TimeoutExpired:
        print(f"⏱ Timeout: {wA_file} vs {wB_file}")
        return None, None, None

    output = proc.stdout + proc.stderr
    m = re.search(r"FINAL:\s*j1_score=(\d+)\s*j2_score=(\d+)\s*winner=([-\d]+)", output)
    if not m:
        print(f"⚠ Erreur match {wA_file} vs {wB_file}")
        print("Output:\n", output)
        return None, None, None
    return int(m.group(1)), int(m.group(2)), int(m.group(3))

# -------------------------------
# Génération des fichiers de poids
# -------------------------------
combo_files = []
combo_weights = []

os.makedirs("weights_tmp", exist_ok=True)

for i in range(N_COMBOS):
    w = randomize_weights(BASE_WEIGHTS, VARIATION)
    fname = os.path.join("weights_tmp", f"weights_{i}.cfg")
    write_weights_file(fname, w)
    combo_files.append(fname)
    combo_weights.append(w)

# -------------------------------
# Tournoi round-robin
# -------------------------------
results = []
scores = {f: 0 for f in combo_files}

for i, wA_file in enumerate(combo_files):
    for j, wB_file in enumerate(combo_files):
        if i == j:
            continue
        for _ in range(N_MATCHES):
            s1, s2, winner = play_match(wA_file, wB_file)
            if winner is None:
                continue
            results.append({
                "botA": wA_file,
                "botB": wB_file,
                "scoreA": s1,
                "scoreB": s2,
                "winner": winner
            })
            if winner == 1:
                scores[wA_file] += 1
            elif winner == 2:
                scores[wB_file] += 1

# -------------------------------
# Sauvegarde CSV
# -------------------------------
with open(OUT_CSV, "w", newline="") as f:
    writer = csv.DictWriter(f, fieldnames=["botA", "botB", "scoreA", "scoreB", "winner"])
    writer.writeheader()
    for r in results:
        writer.writerow(r)
print("CSV écrit :", OUT_CSV)

# -------------------------------
# Graphe
# -------------------------------
plt.figure(figsize=(12,6))
names = [os.path.basename(f) for f in combo_files]
vals = [scores[f] for f in combo_files]
plt.bar(range(len(names)), vals, color="skyblue")
plt.xticks(range(len(names)), names, rotation=90, fontsize=8)
plt.title("Points totaux par combinaison de poids")
plt.ylabel("Points (victoires)")
plt.tight_layout()
plt.savefig(OUT_PNG)
plt.show()
print("Graphe généré :", OUT_PNG)

# -------------------------------
# Meilleure combinaison
# -------------------------------
best_file = max(scores.items(), key=lambda x: x[1])
best_idx = combo_files.index(best_file[0])
best_weights = combo_weights[best_idx]
print("Meilleure combinaison :", best_weights)

# Écriture d’un fichier cfg final "best_weights.cfg"
write_weights_file("best_weights.cfg", best_weights)
print("Fichier 'best_weights.cfg' créé avec la meilleure combinaison")
