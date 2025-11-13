import subprocess
import os
import random
import time

AGENTS_DIR = "agents"
N_MATCHES = 1  # nombre de parties par adversaire
MAX_TURNS = 100 # nombre max de tours (à adapter)
TIMEOUT = 2.0   # secondes max pour répondre

# --- simulation du moteur de jeu (à adapter selon tes règles) ---
def play_match(agent1_path, agent2_path):
    """
    Joue une partie entre agent1 et agent2.
    Communique via stdin/stdout selon ton protocole.
    Retourne : 1 si agent1 gagne, -1 si agent2 gagne, 0 égalité
    """

    # Lance les deux agents
    agent1 = subprocess.Popen(
        [agent1_path], stdin=subprocess.PIPE, stdout=subprocess.PIPE, text=True
    )
    time.sleep(0.1)  # petit délai pour s'assurer que les processus sont prêts
    agent2 = subprocess.Popen(
        [agent2_path], stdin=subprocess.PIPE, stdout=subprocess.PIPE, text=True
    )
    print(f"Match between {os.path.basename(agent1_path)} and {os.path.basename(agent2_path)}")  # debug

    # envoie leur rôle (1 ou 2)
    agent1.stdin.write("1\n")
    agent1.stdin.flush()
    agent2.stdin.write("2\n")
    agent2.stdin.flush()
    print("Assigned roles to agents.")  # debug
    last_move_p1 = "NONE"
    last_move_p2 = "NONE"
    last_move_p1 = agent1.stdout.readline().strip()  # premier coup de l'agent1
    print(f"Agent1 first move: {last_move_p1}")  # debug
    
    for turn in range(MAX_TURNS):
        # --- joueur 1 ---
        agent2.stdin.write(f"{last_move_p1}\n")
        agent2.stdin.flush()
        try:
            move2 = agent2.stdout.readline().strip()
            print(f"Agent2 move: {move2}")  # debug
        except Exception:
            print("Agent2 failed to respond.")
            move2 = "INVALID"

        if not move2:
            # agent1 ne répond pas
            return -1

        # --- joueur 2 ---
        agent1.stdin.write(f"{move2}\n")
        agent1.stdin.flush()
        try:
            move1 = agent1.stdout.readline().strip()
            print(f"Agent1 move: {move1}")  # debug
        except Exception:
            print("Agent1 failed to respond.")
            move1 = "INVALID"

        if not move1:
            # agent2 ne répond pas
            return 1

        # ici tu peux vérifier si un coup est invalide / état de victoire
        # pour ce script on simule le résultat aléatoirement à la fin

        last_move_p1, last_move_p2 = move1, move2

    # --- fin de partie fictive (à remplacer par ta vraie logique) ---
    return random.choice([1, 0, -1])
# -----------------------------------------------------------------

def main():
    agents = sorted([
        os.path.join(AGENTS_DIR, f)
        for f in os.listdir(AGENTS_DIR)
        if os.access(os.path.join(AGENTS_DIR, f), os.X_OK)
    ])

    if len(agents) < 2:
        # print("⚠️  Il faut au moins deux versions d'agents dans le dossier.")
        
        # return
        #on prends le binaire par défaut
        agents = ["./aigame", "./aigame"]

    new_agent = agents[-1]
    print(f"Nouvelle version : {os.path.basename(new_agent)}\n")

    for old_agent in agents[:-1]:
        wins = losses = draws = 0

        for i in range(N_MATCHES):
            # alterner qui commence
            if i % 2 == 0:
                result = play_match(new_agent, old_agent)
            else:
                r = play_match(old_agent, new_agent)
                result = -r  # inverse le point de vue

            if result == 1:
                wins += 1
            elif result == -1:
                losses += 1
            else:
                draws += 1

        print(f"Contre {os.path.basename(old_agent):<15} "
              f"→ {wins} victoires, {losses} défaites, {draws} égalités")

if __name__ == "__main__":
    main()
