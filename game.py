import pygame
import threading
import queue
import sys
import math
import time
import subprocess
import os

# =============================
# Configuration et utilitaires
# =============================
FPS = 30
WIN_W, WIN_H = 1000, 700
HOLE_RADIUS = 30
FONT_SIZE = 18

# couleurs
COLOR_BG = (30, 30, 30)
COLOR_TEXT = (240, 240, 240)
COLOR_HOLE = (200, 180, 120)
COLOR_RED = (200, 40, 40)
COLOR_BLUE = (50, 80, 200)
COLOR_T = (0, 0, 0)  # Noir pour les graines transparentes
COLOR_HIGHLIGHT = (220, 220, 70)
COLOR_GREEN = (40, 200, 40)

# Initial seeds per hole (2R, 2B, 2T)
INIT_PER_COLOR = {'R': 2, 'B': 2, 'T': 2}

# Bot configuration
BOT_ENABLED = False
BOT_PLAYER = 2  # Le bot joue en tant que joueur 2 par défaut
bot_process = None
bot_output_queue = queue.Queue()
bot_input_queue = queue.Queue()

# =============================
# Etat du jeu
# =============================
def make_init_board():
    return [{'R': INIT_PER_COLOR['R'], 'B': INIT_PER_COLOR['B'], 'T': INIT_PER_COLOR['T']} for _ in range(16)]

holes = make_init_board()
captured = {1: 0, 2: 0}
current_player = 1
move_queue = queue.Queue()
game_over = False
winner = None
message = "Bienvenue! Cliquez sur un trou pour jouer ou utilisez la console."
selected_hole = None  # Pour la sélection via GUI
waiting_for_bot = False

# Helper functions
def owner_of(hole_index_0based):
    num = hole_index_0based + 1
    return 1 if num % 2 == 1 else 2

def total_seeds_on_board():
    return sum(h['R'] + h['B'] + h['T'] for h in holes)

def seeds_of_player_on_board(player):
    idxs = [i for i in range(16) if owner_of(i) == player]
    return sum(holes[i]['R'] + holes[i]['B'] + holes[i]['T'] for i in idxs)

def next_index_clockwise(i):
    return (i + 1) % 16

def prev_index_clockwise(i):
    return (i - 1) % 16

def is_opponent_hole(idx, player):
    return owner_of(idx) != player

def is_own_hole(idx, player):
    return owner_of(idx) == player

def available_colors_in_hole(idx):
    cols = []
    if holes[idx]['R'] > 0:
        cols.append('R')
    if holes[idx]['B'] > 0:
        cols.append('B')
    if holes[idx]['T'] > 0:
        cols.append('T')
    return cols

def valid_move_input(s):
    s = s.strip().upper() # Normalisation
    #on enleve les espaces a l'interieur
    s = s.replace(" ","")
    # print(f"Validating move input: '{s}'")
    if len(s) < 2:
        return False
    i = 0
    while i < len(s) and s[i].isdigit():
        i += 1
    if i == 0:
        return False
    num = int(s[:i])
    if not (1 <= num <= 16):
        return False
    rest = s[i:]
    if rest == 'R' or rest == 'B':
        return True
    if rest.startswith('T') and len(rest) == 2 and rest[1] in ('R', 'B'):
        return True
    return False

def parse_move(s):
    s = s.strip().upper()
    s = s.replace(" ","")
    i = 0
    while i < len(s) and s[i].isdigit():
        i += 1
    num = int(s[:i])
    rest = s[i:]
    if rest in ('R','B'):
        return (num - 1, rest, None)
    if rest.startswith('T') and len(rest) == 2 and rest[1] in ('R','B'):
        return (num - 1, 'T', rest[1])
    raise ValueError("Format invalide")

def perform_move(player, hole_idx, color_choice, t_behavior):
    global holes, captured, current_player, game_over, winner

    if owner_of(hole_idx) != player:
        return False, "Ce trou n'appartient pas au joueur."

    if color_choice == 'T':
        if holes[hole_idx]['T'] == 0:
            return False, "Aucune graine transparente dans ce trou."
        use_behavior = t_behavior
    else:
        if holes[hole_idx][color_choice] == 0:
            return False, f"Aucune graine {color_choice} dans ce trou."
        use_behavior = color_choice

    t_to_distribute = 0
    other_to_distribute = 0
    if color_choice == 'T':
        t_to_distribute = holes[hole_idx]['T']
        holes[hole_idx]['T'] = 0
        other_to_distribute = holes[hole_idx][use_behavior]
        holes[hole_idx][use_behavior] = 0
    else:
        other_to_distribute = holes[hole_idx][color_choice]
        holes[hole_idx][color_choice] = 0
        t_to_distribute = 0

    last_idx = None

    def distribute(count, col_kind, distribution_like, start_from):
        if count <= 0:
            return None
        i = start_from
        placed_last = None
        while count > 0:
            i = next_index_clockwise(i)
            if i == hole_idx:
                continue
            if distribution_like == 'B' and not is_opponent_hole(i, player):
                continue
            holes[i][col_kind] += 1
            placed_last = i
            count -= 1
        return placed_last

    cur_start = hole_idx
    if t_to_distribute > 0:
        last_idx = distribute(t_to_distribute, 'T', use_behavior, cur_start)
        if last_idx is not None:
            cur_start = last_idx

    if other_to_distribute > 0:
        last_after_color = distribute(other_to_distribute, use_behavior, use_behavior, cur_start)
        if last_after_color is not None:
            last_idx = last_after_color

    if last_idx is None:
        return False, "Aucun semis effectué."

    captured_this_move = 0
    ci = last_idx
    chain = []
    while True:
        total = holes[ci]['R'] + holes[ci]['B'] + holes[ci]['T']
        if total == 2 or total == 3:
            captured_this_move += total
            chain.append((ci, total))
            holes[ci]['R'] = holes[ci]['B'] = holes[ci]['T'] = 0
            ci = prev_index_clockwise(ci)
        else:
            break

    captured[player] += captured_this_move

    opponent = 1 if player == 2 else 2
    opp_seeds_left = seeds_of_player_on_board(opponent)
    if opp_seeds_left == 0:
        remaining = 0
        for i in range(16):
            s = holes[i]['R'] + holes[i]['B'] + holes[i]['T']
            remaining += s
            holes[i]['R'] = holes[i]['B'] = holes[i]['T'] = 0
        captured[player] += remaining
        if remaining > 0:
            chain.append(("starvation_collect", remaining))

    board_total = total_seeds_on_board()
    if captured[player] >= 49:
        game_over = True
        winner = player
    if captured[1] >= 40 and captured[2] >= 40:
        game_over = True
        winner = 0
    if board_total < 10:
        game_over = True
        if captured[1] > captured[2]:
            winner = 1
        elif captured[2] > captured[1]:
            winner = 2
        else:
            winner = 0

    if not game_over:
        current_player = 1 if player == 2 else 2

    msg = f"Joueur {player} a joué trou {hole_idx+1} {('T as '+use_behavior) if color_choice=='T' else color_choice}."
    if captured_this_move > 0:
        msg += f" Capturé {captured_this_move} graines."
    if len(chain) > 0 and chain[-1][0] == "starvation_collect":
        msg += f" (Starvation: +{chain[-1][1]} restantes)."
    return True, msg

# =============================
# Bot communication threads
# =============================
def bot_output_reader(process):
    """Thread qui lit la sortie du bot (les coups qu'il joue)"""
    while True:
        try:
            line = process.stdout.readline()
            if not line:
                break
            line = line.strip()
            if line:
                bot_output_queue.put(line)
                print(f"[BOT OUTPUT] {line}")
        except:
            break

def bot_stderr_reader(process):
    """Thread qui lit la sortie d'erreur du bot"""
    while True:
        try:
            line = process.stderr.readline()
            if not line:
                break
            line = line.strip()
            if line:
                print(f"[BOT STDERR] {line}")
        except:
            break

def bot_input_writer(process):
    """Thread qui envoie les coups au bot"""
    while True:
        try:
            move = bot_input_queue.get()
            if move is None:  # Signal d'arrêt
                break
            process.stdin.write(move + '\n')
            process.stdin.flush()
            print(f"[BOT INPUT] {move}")
        except:
            break

def start_bot(bot_path='./aigame'):
    global bot_process, BOT_ENABLED
    try:
        bot_process = subprocess.Popen(
            [bot_path, str(BOT_PLAYER)],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            bufsize=1
        )
        
        # Démarrer les threads de communication
        threading.Thread(target=bot_output_reader, args=(bot_process,), daemon=True).start()
        threading.Thread(target=bot_stderr_reader, args=(bot_process,), daemon=True).start()
        threading.Thread(target=bot_input_writer, args=(bot_process,), daemon=True).start()
        
        BOT_ENABLED = True
        print(f"Bot démarré avec succès (Joueur {BOT_PLAYER})")
        return True
    except Exception as e:
        print(f"Erreur lors du démarrage du bot: {e}")
        BOT_ENABLED = False
        return False

def stop_bot():
    global bot_process, BOT_ENABLED
    if bot_process:
        bot_input_queue.put(None)  # Signal d'arrêt
        bot_process.terminate()
        bot_process.wait(timeout=2)
        bot_process = None
    BOT_ENABLED = False

def send_move_to_bot(move_str):
    """Envoie un coup au bot"""
    if BOT_ENABLED and bot_process:
        bot_input_queue.put(move_str)

def check_bot_move():
    """Vérifie si le bot a envoyé un coup"""
    try:
        move = bot_output_queue.get_nowait()
        return move
    except queue.Empty:
        return None

# =============================
# Console input thread
# =============================
def console_input_thread():
    global move_queue
    print("Instructions: entrez un coup comme 3R, 14B, 4TR. Tapez 'bot' pour activer/désactiver le bot. Tapez 'quit' pour quitter.")
    while True:
        try:
            s = input(">> ")
        except EOFError:
            break
        s = s.strip()
        if s.lower() in ('quit','exit'):
            move_queue.put(('quit', None))
            return
        if s.lower() == 'bot':
            move_queue.put(('toggle_bot', None))
            continue
        if s == '':
            continue
        move_queue.put(('move', s))

# =============================
# Pygame rendering
# =============================
pygame.init()
screen = pygame.display.set_mode((WIN_W, WIN_H))
pygame.display.set_caption("Jeu 16-trous (R/B/T) - Pygame avec Bot IA")
clock = pygame.time.Clock()
font = pygame.font.SysFont(None, FONT_SIZE)
font_small = pygame.font.SysFont(None, 14)

center_x, center_y = WIN_W // 2, WIN_H // 2 - 20
radius = min(WIN_W, WIN_H) // 2 - 120
hole_positions = []
for i in range(16):
    angle = -math.pi/2 + 2*math.pi * (i / 16.0)
    x = center_x + int(radius * math.cos(angle))
    y = center_y + int(radius * math.sin(angle))
    hole_positions.append((x, y))

def draw():
    screen.fill(COLOR_BG)
    
    # Draw holes
    for idx, (x,y) in enumerate(hole_positions):
        # Highlight si c'est le trou sélectionné
        if selected_hole == idx:
            pygame.draw.circle(screen, COLOR_HIGHLIGHT, (x,y), HOLE_RADIUS + 5)
        
        # Highlight si c'est un trou du joueur actuel
        if owner_of(idx) == current_player and not game_over:
            pygame.draw.circle(screen, COLOR_GREEN, (x,y), HOLE_RADIUS + 2)
        
        pygame.draw.circle(screen, COLOR_HOLE, (x,y), HOLE_RADIUS)
        pygame.draw.circle(screen, (0,0,0), (x,y), HOLE_RADIUS, 2)
        
        # Hole number
        num_surf = font.render(str(idx+1), True, COLOR_TEXT)
        screen.blit(num_surf, (x - num_surf.get_width()//2, y - HOLE_RADIUS - 18))
        
        # Draw counts
        r_count = holes[idx]['R']
        b_count = holes[idx]['B']
        t_count = holes[idx]['T']
        offset = -10
        if r_count > 0:
            r_surf = font.render(f"R:{r_count}", True, COLOR_RED)
            screen.blit(r_surf, (x - r_surf.get_width()//2, y + offset - 12))
            offset += 14
        if b_count > 0:
            b_surf = font.render(f"B:{b_count}", True, COLOR_BLUE)
            screen.blit(b_surf, (x - b_surf.get_width()//2, y + offset - 12))
            offset += 14
        if t_count > 0:
            t_surf = font.render(f"T:{t_count}", True, COLOR_T)
            screen.blit(t_surf, (x - t_surf.get_width()//2, y + offset - 12))

    # Draw info
    info_x = 30
    info_y = 30
    
    player_text = f"Tour: Joueur {current_player}"
    if BOT_ENABLED and current_player == BOT_PLAYER:
        player_text += " (BOT)"
    elif current_player == 1:
        player_text += " (impair)"
    else:
        player_text += " (pair)"
    
    cp_surf = font.render(player_text, True, COLOR_TEXT)
    screen.blit(cp_surf, (info_x, info_y))
    
    cap_surf = font.render(f"Capturés - Joueur1: {captured[1]}    Joueur2: {captured[2]}", True, COLOR_TEXT)
    screen.blit(cap_surf, (info_x, info_y + 30))
    
    board_surf = font.render(f"Graines sur plateau: {total_seeds_on_board()}", True, COLOR_TEXT)
    screen.blit(board_surf, (info_x, info_y + 60))
    
    # Bot status
    bot_status = f"Bot: {'ACTIVÉ' if BOT_ENABLED else 'DÉSACTIVÉ'} (Joueur {BOT_PLAYER})" if BOT_ENABLED else "Bot: DÉSACTIVÉ (B pour activer)"
    bot_surf = font.render(bot_status, True, COLOR_GREEN if BOT_ENABLED else COLOR_TEXT)
    screen.blit(bot_surf, (info_x, info_y + 90))
    
    msg_surf = font.render(message, True, COLOR_TEXT)
    screen.blit(msg_surf, (info_x, info_y + 120))

    if selected_hole is not None:
        colors = available_colors_in_hole(selected_hole)
        if colors:
            sel_text = f"Trou {selected_hole+1} sélectionné - Appuyez R, B ou T"
            sel_surf = font.render(sel_text, True, COLOR_HIGHLIGHT)
            screen.blit(sel_surf, (info_x, info_y + 150))

    if game_over:
        if winner == 0:
            end_text = "PARTIE TERMINEE : ÉGALITÉ"
        else:
            end_text = f"PARTIE TERMINEE : JOUEUR {winner} GAGNE"
        big = pygame.font.SysFont(None, 36)
        end_s = big.render(end_text, True, COLOR_HIGHLIGHT)
        screen.blit(end_s, (WIN_W//2 - end_s.get_width()//2, WIN_H - 80))

    pygame.display.flip()

def process_move(move_str):
    global message, selected_hole, waiting_for_bot
    
    # Identifier qui joue ce coup
    is_bot_move = (BOT_ENABLED and current_player == BOT_PLAYER)
    
    if not valid_move_input(move_str):
        message = "Format invalide. Exemples: 3R, 14B, 4TR"
        print(message)
        return False
    
    try:
        hole_idx, color_choice, t_beh = parse_move(move_str)
    except Exception as e:
        message = f"Erreur parse: {e}"
        print(message)
        return False

    if owner_of(hole_idx) != current_player:
        message = f"Erreur: trou {hole_idx+1} n'appartient pas au joueur {current_player}."
        print(message)
        return False

    ok, msg = perform_move(current_player, hole_idx, color_choice, t_beh)
    message = msg
    print(msg)
    selected_hole = None
    
    if ok:
        # CORRECTION: Envoyer le coup au bot SEULEMENT si ce n'est PAS le bot qui vient de jouer
        if BOT_ENABLED and not is_bot_move:
            send_move_to_bot(move_str)
        
        # Si c'est maintenant au tour du bot, marquer qu'on attend
        if BOT_ENABLED and current_player == BOT_PLAYER and not game_over:
            waiting_for_bot = True
            message += " (En attente du bot...)"
    
    return ok

def handle_hole_click(hole_idx):
    global selected_hole, message
    
    if game_over:
        message = "La partie est terminée."
        return
    
    if owner_of(hole_idx) != current_player:
        message = f"Ce trou appartient au joueur {owner_of(hole_idx)}, pas à vous!"
        return
    
    # Si c'est le tour du bot, ne pas permettre de clic
    if BOT_ENABLED and current_player == BOT_PLAYER:
        message = "C'est au tour du bot!"
        return
    
    colors = available_colors_in_hole(hole_idx)
    if not colors:
        message = f"Trou {hole_idx+1} est vide!"
        return
    
    selected_hole = hole_idx
    message = f"Trou {hole_idx+1} sélectionné. Appuyez sur R, B ou T pour choisir la couleur."

def handle_color_choice(key):
    global selected_hole, message
    
    if selected_hole is None:
        message = "Sélectionnez d'abord un trou en cliquant dessus."
        return
    
    colors = available_colors_in_hole(selected_hole)
    
    if key == pygame.K_r and 'R' in colors:
        move_str = f"{selected_hole+1}R"
        process_move(move_str)
    elif key == pygame.K_b and 'B' in colors:
        move_str = f"{selected_hole+1}B"
        process_move(move_str)
    elif key == pygame.K_t and 'T' in colors:
        message = "Transparent sélectionné. Appuyez sur R ou B pour le comportement."
    else:
        message = f"Couleur non disponible dans le trou {selected_hole+1}."

def handle_transparent_behavior(key):
    global selected_hole, message
    
    if selected_hole is None:
        return
    
    colors = available_colors_in_hole(selected_hole)
    if 'T' not in colors:
        return
    
    if key == pygame.K_r:
        move_str = f"{selected_hole+1}TR"
        process_move(move_str)
    elif key == pygame.K_b:
        move_str = f"{selected_hole+1}TB"
        process_move(move_str)

# =============================
# Main loop
# =============================
inp_thread = threading.Thread(target=console_input_thread, daemon=True)
inp_thread.start()

print("=== Jeu Oware avec Bot IA ===")
print("Appuyez sur 'B' dans la fenêtre pour activer/désactiver le bot")
print("Cliquez sur un trou puis appuyez R/B/T pour jouer")

running = True
waiting_for_transparent_behavior = False

while running:
    # Pygame events
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
            move_queue.put(('quit', None))
        
        elif event.type == pygame.MOUSEBUTTONDOWN and not game_over:
            mx, my = event.pos
            for idx, (x, y) in enumerate(hole_positions):
                dist = math.sqrt((mx - x)**2 + (my - y)**2)
                if dist <= HOLE_RADIUS:
                    handle_hole_click(idx)
                    break
        
        elif event.type == pygame.KEYDOWN and not game_over:
            if event.key == pygame.K_b and not waiting_for_transparent_behavior:
                if pygame.key.get_mods() & pygame.KMOD_CTRL:
                    # Ctrl+B pour toggle bot
                    if BOT_ENABLED:
                        stop_bot()
                        message = "Bot désactivé"
                    else:
                        if start_bot():
                            message = f"Bot activé (Joueur {BOT_PLAYER})"
                        else:
                            message = "Erreur: impossible de démarrer le bot"
                else:
                    # B pour jouer la couleur bleue
                    handle_color_choice(event.key)
            elif event.key == pygame.K_b:
                if waiting_for_transparent_behavior:
                    handle_transparent_behavior(event.key)
                    waiting_for_transparent_behavior = False
                else:
                    handle_color_choice(event.key)
            elif event.key == pygame.K_r:
                if waiting_for_transparent_behavior:
                    handle_transparent_behavior(event.key)
                    waiting_for_transparent_behavior = False
                else:
                    handle_color_choice(event.key)
            elif event.key == pygame.K_t:
                colors = available_colors_in_hole(selected_hole) if selected_hole is not None else []
                if 'T' in colors:
                    waiting_for_transparent_behavior = True
                    message = f"Trou {selected_hole+1}T - Appuyez R ou B pour le comportement."

    # Check console input
    try:
        typ, payload = move_queue.get_nowait()
        if typ == 'quit':
            running = False
            break
        elif typ == 'toggle_bot':
            if BOT_ENABLED:
                stop_bot()
                message = "Bot désactivé"
                print(message)
            else:
                if start_bot():
                    message = f"Bot activé (Joueur {BOT_PLAYER})"
                    print(message)
                else:
                    message = "Erreur: impossible de démarrer le bot"
                    print(message)
        elif typ == 'move':
            if game_over:
                message = "La partie est terminée."
                print(message)
            else:
                process_move(payload)
    except queue.Empty:
        pass

    # Check bot move
    if BOT_ENABLED and current_player == BOT_PLAYER and not game_over and waiting_for_bot:
        bot_move = check_bot_move()
        if bot_move:
            print(f"Le bot joue: {bot_move}")
            message = f"Bot joue: {bot_move}"
            process_move(bot_move)
            waiting_for_bot = False

    draw()
    clock.tick(FPS)

stop_bot()
pygame.quit()
print("Fenêtre fermée. Fin du programme.")