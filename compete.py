import subprocess
import os
import random
import time

AGENTS_DIR = "agents"
N_MATCHES = 2  # nombre de parties par adversaire
MAX_TURNS = 450 # nombre max de tours (à adapter)
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
            # Détection de sortie corrompue/invalide
            if "Invalid" in move2 or any(ord(c) > 127 for c in move2):
                print(f"Agent2 produced invalid/corrupted output: {move2}")
                agent1.kill()
                agent2.kill()
                return -1
        except Exception as e:
            print(f"Agent2 failed to respond: {e}")
            agent1.kill()
            agent2.kill()
            move2 = "INVALID"
            return -1

        if not move2:
            # agent2 ne répond pas
            agent1.kill()
            agent2.kill()
            return -1
        if move2 == "Player 1 wins!":
            agent1.kill()
            agent2.kill()
            return 1
        if move2 == "Player 2 wins!":
            agent1.kill()
            agent2.kill()
            return -1
        if move2 == "Game ends in a draw!":
            agent1.kill()
            agent2.kill()
            return 0
        # --- joueur 2 ---
        agent1.stdin.write(f"{move2}\n")
        agent1.stdin.flush()
        try:
            move1 = agent1.stdout.readline().strip()
            print(f"Agent1 move: {move1}")  # debug
            # Détection de sortie corrompue/invalide
            if "Invalid" in move1 or any(ord(c) > 127 for c in move1):
                print(f"Agent1 produced invalid/corrupted output: {move1}")
                agent1.kill()
                agent2.kill()
                return 1
        except Exception as e:
            print(f"Agent1 failed to respond: {e}")
            agent1.kill()
            agent2.kill()
            move1 = "INVALID"
            return 1

        if not move1:
            # agent1 ne répond pas
            agent1.kill()
            agent2.kill()
            return 1
        if move1 == "Player 1 wins!":
            agent1.kill()
            agent2.kill()
            return 1
        if move1 == "Player 2 wins!":
            agent1.kill()
            agent2.kill()
            return -1
        if move1 == "Game ends in a draw!":
            agent1.kill()
            agent2.kill()
            return 0
        # ici tu peux vérifier si un coup est invalide / état de victoire
        # pour ce script on simule le résultat aléatoirement à la fin

        last_move_p1, last_move_p2 = move1, move2

    if turn > MAX_TURNS - 1:
        print("Max turns reached, ending match.")  # debug
        result = 0  # match nul si max turns atteint

    # # --- fin de partie fictive (à remplacer par ta vraie logique) ---
    # result = random.choice([1, 0, -1])
    
    # Cleanup: kill both processes at the end
    try:
        agent1.kill()
        agent2.kill()
    except:
        pass
    
    return result
# -----------------------------------------------------------------

def main():
    # Récupération de tous les binaires disponibles
    agents = sorted([
        os.path.join(AGENTS_DIR, f)
        for f in os.listdir(AGENTS_DIR)
        if os.access(os.path.join(AGENTS_DIR, f), os.X_OK)
    ])
    
    # Ajouter le binaire par défaut s'il existe
    if os.path.exists("./aigame") and os.access("./aigame", os.X_OK):
        agents.append("./aigame")
        agents = sorted(list(set(agents)))  # Enlever les doublons et trier

    if len(agents) == 0:
        print("❌ Aucun binaire trouvé!")
        print(f"   Vérifiez que des binaires exécutables existent dans '{AGENTS_DIR}/' ou './aigame'")
        return
    
    # Affichage et sélection du binaire à évaluer
    print("="*60)
    print("SÉLECTION DU BINAIRE À ÉVALUER")
    print("="*60)
    print("\nBinaires disponibles :")
    for i, agent in enumerate(agents, 1):
        print(f"  {i}. {agent}")
    
    # Choix de l'utilisateur
    while True:
        try:
            choice = input(f"\nChoisissez le binaire à évaluer (1-{len(agents)}, ou Entrée pour le dernier) : ").strip()
            if choice == "":
                new_agent = agents[-1]
                break
            choice_idx = int(choice) - 1
            if 0 <= choice_idx < len(agents):
                new_agent = agents[choice_idx]
                break
            else:
                print(f"❌ Choisissez un nombre entre 1 et {len(agents)}")
        except ValueError:
            print("❌ Entrée invalide, entrez un nombre")
        except KeyboardInterrupt:
            print("\n\n❌ Annulé par l'utilisateur")
            return
    
    # Créer la liste des adversaires (tous les binaires, y compris lui-même)
    opponents = agents.copy()
    
    print(f"\n{'='*60}")
    print(f"Binaire évalué : {new_agent}")
    print(f"Adversaires    : {len(opponents)} binaire(s)")
    print(f"Matchs/adversaire : {N_MATCHES}")
    print(f"{'='*60}\n")
    
    time.sleep(1)  # Petit délai pour lire

    # Statistiques globales
    total_wins = 0
    total_losses = 0
    total_draws = 0
    total_matches = 0
    results_per_opponent = []

    for old_agent in opponents:
        wins = losses = draws = 0
        wins_as_p1 = losses_as_p1 = draws_as_p1 = 0
        wins_as_p2 = losses_as_p2 = draws_as_p2 = 0

        for i in range(N_MATCHES):
            # Vérifier que N_MATCHES est pair pour garantir l'alternance
            if N_MATCHES % 2 != 0 and i == N_MATCHES - 1:
                print(f"⚠️  Attention: N_MATCHES={N_MATCHES} est impair, dernière partie sans alternance")
            
            # alterner qui commence : une partie sur deux
            if i % 2 == 0:
                # new_agent joue en premier (joueur 1)
                print(f"\n  Match {i+1}/{N_MATCHES}: {os.path.basename(new_agent)} (J1) vs {os.path.basename(old_agent)} (J2)")
                result = play_match(new_agent, old_agent)
                if result == 1:
                    wins += 1
                    wins_as_p1 += 1
                elif result == -1:
                    losses += 1
                    losses_as_p1 += 1
                else:
                    draws += 1
                    draws_as_p1 += 1
            else:
                # new_agent joue en second (joueur 2)
                print(f"\n  Match {i+1}/{N_MATCHES}: {os.path.basename(old_agent)} (J1) vs {os.path.basename(new_agent)} (J2)")
                r = play_match(old_agent, new_agent)
                result = -r  # inverse le point de vue
                if result == 1:
                    wins += 1
                    wins_as_p2 += 1
                elif result == -1:
                    losses += 1
                    losses_as_p2 += 1
                else:
                    draws += 1
                    draws_as_p2 += 1

        print(f"\n→ Contre {os.path.basename(old_agent):<15} "
              f"→ {wins}V / {losses}D / {draws}N "
              f"(J1: {wins_as_p1}V-{losses_as_p1}D-{draws_as_p1}N | J2: {wins_as_p2}V-{losses_as_p2}D-{draws_as_p2}N)")
        
        # Accumuler les statistiques
        total_wins += wins
        total_losses += losses
        total_draws += draws
        total_matches += N_MATCHES
        results_per_opponent.append({
            'opponent': os.path.basename(old_agent),
            'wins': wins,
            'losses': losses,
            'draws': draws,
            'wins_as_p1': wins_as_p1,
            'losses_as_p1': losses_as_p1,
            'draws_as_p1': draws_as_p1,
            'wins_as_p2': wins_as_p2,
            'losses_as_p2': losses_as_p2,
            'draws_as_p2': draws_as_p2
        })
    
    # Affichage du compte rendu final
    print("\n" + "="*60)
    print("COMPTE RENDU FINAL")
    print("="*60)
    print(f"Agent évalué : {os.path.basename(new_agent)}")
    print(f"Nombre total de matchs : {total_matches}")
    print(f"Nombre d'adversaires : {len(opponents)}")
    print("-"*60)
    print(f"{'Résultats globaux':<30} {'Nombre':<10} {'%'}")
    print("-"*60)
    
    win_rate = (total_wins / total_matches * 100) if total_matches > 0 else 0
    loss_rate = (total_losses / total_matches * 100) if total_matches > 0 else 0
    draw_rate = (total_draws / total_matches * 100) if total_matches > 0 else 0
    
    print(f"{'Victoires':<30} {total_wins:<10} {win_rate:>5.1f}%")
    print(f"{'Défaites':<30} {total_losses:<10} {loss_rate:>5.1f}%")
    print(f"{'Égalités':<30} {total_draws:<10} {draw_rate:>5.1f}%")
    print("-"*60)
    
    # Score global (victoires = 1 point, égalités = 0.5 point)
    total_points = total_wins + (total_draws * 0.5)
    max_points = total_matches
    score_percentage = (total_points / max_points * 100) if max_points > 0 else 0
    
    print(f"Score total : {total_points:.1f} / {max_points} ({score_percentage:.1f}%)")
    
    # Détail par adversaire
    print("\n" + "="*60)
    print("DÉTAIL PAR ADVERSAIRE")
    print("="*60)
    print(f"{'Adversaire':<20} {'Total':<15} {'En J1':<15} {'En J2':<15}")
    print("-"*60)
    for result in results_per_opponent:
        total_str = f"{result['wins']}V-{result['losses']}D-{result['draws']}N"
        j1_str = f"{result['wins_as_p1']}V-{result['losses_as_p1']}D-{result['draws_as_p1']}N"
        j2_str = f"{result['wins_as_p2']}V-{result['losses_as_p2']}D-{result['draws_as_p2']}N"
        print(f"{result['opponent']:<20} {total_str:<15} {j1_str:<15} {j2_str:<15}")
    print("-"*60)
    
    # Vérification de l'alternance
    if N_MATCHES % 2 == 0:
        print(f"✓ Alternance respectée : {N_MATCHES//2} matchs en J1, {N_MATCHES//2} matchs en J2 par adversaire")
    else:
        print(f"⚠️  N_MATCHES={N_MATCHES} est impair : alternance non parfaite")
    
    # Évaluation qualitative
    print("\n" + "="*60)
    print("ÉVALUATION")
    print("="*60)
    if score_percentage >= 80:
        evaluation = "Excellent ⭐⭐⭐"
    elif score_percentage >= 60:
        evaluation = "Bon ⭐⭐"
    elif score_percentage >= 40:
        evaluation = "Moyen ⭐"
    else:
        evaluation = "Faible"
    print(f"Performance globale : {evaluation}")
    print("="*60 + "\n")

if __name__ == "__main__":
    main()
