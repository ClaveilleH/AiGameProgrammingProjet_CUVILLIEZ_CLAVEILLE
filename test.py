# game_oware_like.py
import pygame
import threading
import queue
import sys
import math
import time

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
# COLOR_T = (170, 170, 170)
COLOR_T = (0, 0, 0)  # Noir pour les graines transparentes
COLOR_HIGHLIGHT = (220, 220, 70)

# Initial seeds per hole (2R, 2B, 2T)
INIT_PER_COLOR = {'R': 2, 'B': 2, 'T': 2}

# =============================
# Etat du jeu
# =============================
# holes: array of 16 dicts: {'R': int, 'B': int, 'T': int}
def make_init_board():
    return [{'R': INIT_PER_COLOR['R'], 'B': INIT_PER_COLOR['B'], 'T': INIT_PER_COLOR['T']} for _ in range(16)]

holes = make_init_board()
captured = {1: 0, 2: 0}  # player 1 is odd holes, player 2 even holes
current_player = 1  # 1 or 2; player 1 starts
move_queue = queue.Queue()
game_over = False
winner = None
message = "Bienvenue! Jouez avec la console (ex: 3R, 14B, 4TR)."

# Helper: hole ownership: player 1 -> odd numbers 1,3,...15 ; player2 -> evens 2,4,...16
def owner_of(hole_index_0based):
    num = hole_index_0based + 1
    return 1 if num % 2 == 1 else 2

def total_seeds_on_board():
    return sum(h['R'] + h['B'] + h['T'] for h in holes)

def seeds_of_player_on_board(player):
    idxs = [i for i in range(16) if owner_of(i) == player]
    return sum(holes[i]['R'] + holes[i]['B'] + holes[i]['T'] for i in idxs)

# =============================
# Sowing & capture logic
# =============================
def next_index_clockwise(i):
    return (i + 1) % 16

def prev_index_clockwise(i):
    return (i - 1) % 16

def is_opponent_hole(idx, player):
    return owner_of(idx) != player

def is_own_hole(idx, player):
    return owner_of(idx) == player

def valid_move_input(s):
    s = s.strip().upper()
    if len(s) < 2:
        return False
    # parse like "3R" or "4TR"
    # find leading digits
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
    # returns (hole_index0, chosen_color, transparent_behavior)
    # chosen_color is 'R' or 'B' or 'T'
    # if chosen_color == 'T', transparent_behavior is 'R' or 'B'
    s = s.strip().upper()
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

def available_colors_in_hole(idx):
    cols = []
    if holes[idx]['R'] > 0:
        cols.append('R')
    if holes[idx]['B'] > 0:
        cols.append('B')
    if holes[idx]['T'] > 0:
        cols.append('T')
    return cols

# def perform_move(player, hole_idx, color_choice, t_behavior):
#     """
#     player: 1 or 2
#     hole_idx: 0-based
#     color_choice: 'R' or 'B' or 'T'
#     t_behavior: if color_choice == 'T', then 'R' or 'B', else None
#     returns (success:bool, message:str)
#     """
#     global holes, captured, current_player, game_over, winner

#     if owner_of(hole_idx) != player:
#         return False, "Ce trou n'appartient pas au joueur."

#     # Check there are seeds of that color (for T, there must be T seeds)
#     if color_choice == 'T':
#         if holes[hole_idx]['T'] == 0:
#             return False, "Aucune graine transparente dans ce trou."
#         use_behavior = t_behavior
#     else:
#         if holes[hole_idx][color_choice] == 0:
#             return False, f"Aucune graine {color_choice} dans ce trou."
#         use_behavior = color_choice

#     # Remove seeds from hole
#     t_to_distribute = holes[hole_idx]['T'] if color_choice == 'T' else 0
#     other_to_distribute = 0
#     if color_choice == 'T':
#         # transparent seeds remain 'T' in destination, but follow distribution rules of use_behavior
#         holes[hole_idx]['T'] = 0
#         # Note: you do NOT play the other colors unless color_choice indicates
#         # So only T seeds are removed in this case
#         # WAIT: rules say transparent are distributed first and "then they are played with the other seeds of the same color"
#         # That suggests that if hole had also the designated color seeds, they are played together.
#         # Example 4TR: transparent considered as red and they are played with the other reds.
#         # So if there are reds also in the same hole, they also get played.
#         other_to_distribute = holes[hole_idx][use_behavior]
#         holes[hole_idx][use_behavior] = 0
#     else:
#         # color_choice is R or B; we may also have transparent in the hole in which case transparent are NOT removed
#         # unless player explicitly chooses T. According to rules: "transparent remains transparent and are distributed first (that is before the other seeds of the designated color)."
#         # That applies only when transparent is selected. So we only remove chosen color seeds.
#         other_to_distribute = holes[hole_idx][color_choice]
#         holes[hole_idx][color_choice] = 0
#         t_to_distribute = 0

#     # Sowing: first distribute transparent seeds (t_to_distribute), then other_to_distribute
#     last_idx = None

#     def distribute(count, col_kind, distribution_like):
#         # distribution_like is 'R' or 'B' indicating which holes are valid
#         # col_kind is the color we put in destination ('T' or 'R' or 'B')
#         nonlocal last_idx
#         i = hole_idx
#         while count > 0:
#             i = next_index_clockwise(i)
#             # skip the starting hole for the selected color: "Seeds are not distributed into the hole drawn from."
#             if i == hole_idx:
#                 continue
#             # filter by distribution rule
#             if distribution_like == 'R':
#                 # distributed in each hole (all allowed)
#                 pass
#             elif distribution_like == 'B':
#                 # only opponent holes
#                 if not is_opponent_hole(i, player):
#                     continue
#             # place seed
#             holes[i][col_kind] += 1
#             last_idx = i
#             count -= 1

#     # First T seeds (if any)
#     if t_to_distribute > 0:
#         distribute(t_to_distribute, 'T', use_behavior)

#     # Then the other seeds (R or B), if any
#     if other_to_distribute > 0:
#         distribute(other_to_distribute, use_behavior, use_behavior)

#     if last_idx is None:
#         # No seeds distributed? Shouldn't happen if input validated earlier
#         return False, "Aucun semis effectué."

#     # After sowing, capturing phase
#     captured_this_move = 0
#     ci = last_idx
#     chain = []
#     # capture while hole has exactly 2 or 3 seeds
#     while True:
#         total = holes[ci]['R'] + holes[ci]['B'] + holes[ci]['T']
#         if total == 2 or total == 3:
#             captured_this_move += total
#             chain.append((ci, total))
#             # empty hole
#             holes[ci]['R'] = holes[ci]['B'] = holes[ci]['T'] = 0
#             ci = prev_index_clockwise(ci)
#         else:
#             break

#     # add to captured
#     captured[player] += captured_this_move

#     # Starvation rule: if opponent has zero seeds on board after capture, last player captures all remaining seeds
#     opponent = 1 if player == 2 else 2
#     opp_seeds_left = seeds_of_player_on_board(opponent)
#     if opp_seeds_left == 0:
#         # collect all seeds left on board
#         remaining = 0
#         for i in range(16):
#             s = holes[i]['R'] + holes[i]['B'] + holes[i]['T']
#             remaining += s
#             holes[i]['R'] = holes[i]['B'] = holes[i]['T'] = 0
#         captured[player] += remaining
#         if remaining > 0:
#             chain.append(("starvation_collect", remaining))

#     # Check end conditions
#     board_total = total_seeds_on_board()
#     # If a player captured >=49 -> win
#     if captured[player] >= 49:
#         game_over = True
#         winner = player
#     # If both have 40 -> draw
#     if captured[1] >= 40 and captured[2] >= 40:
#         game_over = True
#         winner = 0  # draw
#     # If strictly less than 10 seeds on board -> game ends, remaining seeds not counted
#     if board_total < 10:
#         game_over = True
#         # winner is who has more captured
#         if captured[1] > captured[2]:
#             winner = 1
#         elif captured[2] > captured[1]:
#             winner = 2
#         else:
#             winner = 0

#     # Next player's turn unless game over
#     if not game_over:
#         current_player = 1 if player == 2 else 2

#     # Build message summarizing the move
#     msg = f"Joueur {player} a joué trou {hole_idx+1} {('T as '+use_behavior) if color_choice=='T' else color_choice}."
#     if captured_this_move > 0:
#         msg += f" Capturé {captured_this_move} graines."
#     if len(chain) > 0 and chain[-1][0] == "starvation_collect":
#         msg += f" (Starvation: +{chain[-1][1]} restantes)."
#     return True, msg

def perform_move(player, hole_idx, color_choice, t_behavior):
    """
    player: 1 or 2
    hole_idx: 0-based
    color_choice: 'R' or 'B' or 'T'
    t_behavior: if color_choice == 'T', then 'R' or 'B', else None
    returns (success:bool, message:str)
    """
    global holes, captured, current_player, game_over, winner

    if owner_of(hole_idx) != player:
        return False, "Ce trou n'appartient pas au joueur."

    # Check there are seeds of that color (for T, there must be T seeds)
    if color_choice == 'T':
        if holes[hole_idx]['T'] == 0:
            return False, "Aucune graine transparente dans ce trou."
        use_behavior = t_behavior
    else:
        if holes[hole_idx][color_choice] == 0:
            return False, f"Aucune graine {color_choice} dans ce trou."
        use_behavior = color_choice

    # Remove seeds from hole according to the chosen play
    t_to_distribute = 0
    other_to_distribute = 0
    if color_choice == 'T':
        # take all T from the hole and also take the seeds of the designated color
        t_to_distribute = holes[hole_idx]['T']
        holes[hole_idx]['T'] = 0
        other_to_distribute = holes[hole_idx][use_behavior]
        holes[hole_idx][use_behavior] = 0
    else:
        # only take the chosen color seeds
        other_to_distribute = holes[hole_idx][color_choice]
        holes[hole_idx][color_choice] = 0
        t_to_distribute = 0

    last_idx = None

    def distribute(count, col_kind, distribution_like, start_from):
        """
        distribute `count` seeds of kind `col_kind` starting after index start_from.
        distribution_like: 'R' => distribute everywhere; 'B' => distribute only on opponent holes.
        returns last index where a seed was placed (or None)
        """
        if count <= 0:
            return None
        i = start_from
        placed_last = None
        while count > 0:
            i = next_index_clockwise(i)
            # never place into the original hole we took from
            if i == hole_idx:
                continue
            # distribution filter
            if distribution_like == 'B' and not is_opponent_hole(i, player):
                continue
            # place seed
            holes[i][col_kind] += 1
            placed_last = i
            count -= 1
        return placed_last

    # First: distribute transparent seeds (they remain 'T' in destination)
    cur_start = hole_idx
    if t_to_distribute > 0:
        last_idx = distribute(t_to_distribute, 'T', use_behavior, cur_start)
        # next distribution (colored) must continue from the last placed transparent
        if last_idx is not None:
            cur_start = last_idx

    # Then: distribute the chosen color seeds (R or B). They start AFTER the last transparent
    if other_to_distribute > 0:
        # if there were no transparents distributed, cur_start stays as hole_idx
        last_after_color = distribute(other_to_distribute, use_behavior, use_behavior, cur_start)
        if last_after_color is not None:
            last_idx = last_after_color

    if last_idx is None:
        return False, "Aucun semis effectué."

    # After sowing, capturing phase (unchanged)
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

    # Starvation rule
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

    # End conditions (identiques)
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
# Console input thread
# =============================
def console_input_thread():
    global move_queue, game_over, message
    prompt = ">> "
    print("Instructions: entrez un coup comme 3R, 14B, 4TR (T suivi de R ou B). Tapez 'quit' pour quitter.")
    while True:
        try:
            s = input(prompt)
        except EOFError:
            break
        s = s.strip()
        if s.lower() in ('quit','exit'):
            move_queue.put(('quit', None))
            return
        if s == '':
            continue
        move_queue.put(('move', s))

# =============================
# Pygame rendering
# =============================
pygame.init()
screen = pygame.display.set_mode((WIN_W, WIN_H))
pygame.display.set_caption("Jeu 16-trous (R/B/T) - Pygame + Console")
clock = pygame.time.Clock()
font = pygame.font.SysFont(None, FONT_SIZE)

# compute positions around a circle
center_x, center_y = WIN_W // 2, WIN_H // 2 - 20
radius = min(WIN_W, WIN_H) // 2 - 120
hole_positions = []
for i in range(16):
    angle = -math.pi/2 + 2*math.pi * (i / 16.0)  # start top and go clockwise
    x = center_x + int(radius * math.cos(angle))
    y = center_y + int(radius * math.sin(angle))
    hole_positions.append((x, y))

def draw():
    screen.fill(COLOR_BG)
    # draw holes
    for idx, (x,y) in enumerate(hole_positions):
        pygame.draw.circle(screen, COLOR_HOLE, (x,y), HOLE_RADIUS)
        pygame.draw.circle(screen, (0,0,0), (x,y), HOLE_RADIUS, 2)
        # hole number
        num_surf = font.render(str(idx+1), True, COLOR_TEXT)
        screen.blit(num_surf, (x - num_surf.get_width()//2, y - HOLE_RADIUS - 18))
        # draw counts per color as small circles or numbers
        # positions relative inside hole
        r_count = holes[idx]['R']
        b_count = holes[idx]['B']
        t_count = holes[idx]['T']
        # draw small dots aligned vertically
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

    # draw center info: captured counts and current player
    info_x = 30
    info_y = 30
    cp_surf = font.render(f"Tour: Joueur {current_player} ({'impair' if current_player==1 else 'pair'})", True, COLOR_TEXT)
    screen.blit(cp_surf, (info_x, info_y))
    cap_surf = font.render(f"Capturés - Joueur1: {captured[1]}    Joueur2: {captured[2]}", True, COLOR_TEXT)
    screen.blit(cap_surf, (info_x, info_y + 30))
    board_surf = font.render(f"Graines sur plateau: {total_seeds_on_board()}", True, COLOR_TEXT)
    screen.blit(board_surf, (info_x, info_y + 60))
    msg_surf = font.render(message, True, COLOR_TEXT)
    screen.blit(msg_surf, (info_x, info_y + 100))

    if game_over:
        if winner == 0:
            end_text = "PARTIE TERMINEE : ÉGALITÉ"
        else:
            end_text = f"PARTIE TERMINEE : JOUEUR {winner} GAGNE"
        big = pygame.font.SysFont(None, 36)
        end_s = big.render(end_text, True, COLOR_HIGHLIGHT)
        screen.blit(end_s, (WIN_W//2 - end_s.get_width()//2, WIN_H - 80))

    pygame.display.flip()

# =============================
# Main loop + start console thread
# =============================
inp_thread = threading.Thread(target=console_input_thread, daemon=True)
inp_thread.start()

def process_console_move(text):
    global message, game_over
    s = text.strip()
    if not valid_move_input(s):
        message = "Format invalide. Exemples: 3R, 14B, 4TR"
        print(message)
        return
    try:
        hole_idx, color_choice, t_beh = parse_move(s)
    except Exception as e:
        message = f"Erreur parse: {e}"
        print(message)
        return

    # Check ownership
    if owner_of(hole_idx) != current_player:
        message = f"Erreur: trou {hole_idx+1} n'appartient pas au joueur {current_player}."
        print(message)
        return

    # If color_choice is T and t_beh is None -> invalid (shouldn't happen thanks to parser)
    ok, msg = perform_move(current_player, hole_idx, color_choice, t_beh)
    message = msg
    print(msg)
    if not ok:
        # if invalid move, do not switch player
        return

    # print board summary
    print_board_simple()

def print_board_simple():
    # affiche rapidement le plateau dans la console
    rows = []
    for i in range(16):
        h = holes[i]
        rows.append(f"{i+1}:R{h['R']}B{h['B']}T{h['T']}")
    print("Plateau:", " | ".join(rows))
    print(f"Capturés: Joueur1={captured[1]}  Joueur2={captured[2]}")
    print(f"Graines sur plateau: {total_seeds_on_board()}")
    if game_over:
        if winner == 0:
            print("Partie terminée: égalité.")
        else:
            print(f"Partie terminée: joueur {winner} a gagné.")

# initial print
print_board_simple()

# Main pygame loop
running = True
while running:
    # Pygame events
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
            move_queue.put(('quit', None))

    # Check console move queue
    try:
        typ, payload = move_queue.get_nowait()
        if typ == 'quit':
            running = False
            break
        if typ == 'move':
            if game_over:
                message = "La partie est terminée. Redémarrez pour jouer encore."
                print(message)
            else:
                process_console_move(payload)
    except queue.Empty:
        pass

    draw()
    clock.tick(FPS)

pygame.quit()
print("Fenêtre fermée. Fin du programme.")
