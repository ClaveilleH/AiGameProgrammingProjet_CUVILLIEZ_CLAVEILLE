#!/usr/bin/env python3
import subprocess
import csv
import itertools
import matplotlib.pyplot as plt

# ------------------------------------
# PARAMÈTRES
# ------------------------------------
BINARY = "./aigame"  # ton exécutable
WEIGHTS = [
    "weights1.cfg",
    "weights2.cfg",
    "weights3.cfg",
    "weights4.cfg",
]  # <-- mets ici tes fichiers
OUT_CSV = "tournament_results.csv"
OUT_PNG = "scores.png"

# ------------------------------------
# Fonction pour jouer un match
# ------------------------------------
def play_match(wA, wB):
    cmd = [BINARY, "autoplay", wA, wB]

    proc = subprocess.run(cmd, capture_output=True, text=True)
    output = proc.stdout + proc.stderr

    import re
    m = re.search(r"FINAL:\s*j1_score=(\d+)\s*j2_score=(\d+)\s*winner=([-\d]+)", output)

    if not m:
        print("⚠ Erreur lors du match", wA, "vs", wB)
        return None, None, None

    s1 = int(m.group(1))
    s2 = int(m.group(2))
    win = int(m.group(3))

    return s1, s2, win

# ------------------------------------
# Tournoi complet
# ------------------------------------
results = []
scores = {w: 0 for w in WEIGHTS}

pairs = list(itertools.permutations(WEIGHTS, 2))

for wA, wB in pairs:
    print(f"Match {wA} vs {wB}...")
    s1, s2, winner = play_match(wA, wB)

    results.append({
        "botA": wA,
        "botB": wB,
        "scoreA": s1,
        "scoreB": s2,
        "winner": winner,
    })

    # Système de points :
    # 1 = botA win → +1
    # 2 = botB win → +1
    if winner == 1:
        scores[wA] += 1
    elif winner == 2:
        scores[wB] += 1

# ------------------------------------
# Sauvegarde CSV
# ------------------------------------
with open(OUT_CSV, "w", newline="") as f:
    writer = csv.DictWriter(f, fieldnames=["botA", "botB", "scoreA", "scoreB", "winner"])
    writer.writeheader()
    for r in results:
        writer.writerow(r)

print("CSV écrit :", OUT_CSV)

# ------------------------------------
# Génération du graphe
# ------------------------------------
names = list(scores.keys())
vals  = list(scores.values())

plt.figure(figsize=(10, 6))
plt.bar(names, vals)
plt.xticks(rotation=45, ha="right")
plt.title("Score total des bots (tournoi round robin)")
plt.ylabel("Points (victoires)")
plt.tight_layout()
plt.savefig(OUT_PNG)

print("Graphe généré :", OUT_PNG)
