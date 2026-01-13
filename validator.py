class GameValidator:
    def __init__(self):
        # 16 trous avec 2R, 2B, 2T chacun au départ
        self.holes = [{'R': 2, 'B': 2, 'T': 2} for _ in range(16)]
        self.score_A = 0  # Joueur A: trous impairs (1,3,5,7,9,11,13,15)
        self.score_B = 0  # Joueur B: trous pairs (2,4,6,8,10,12,14,16)
        self.move_count = 0
        
    def get_total_seeds_in_hole(self, hole_idx):
        return sum(self.holes[hole_idx].values())
    
    def get_total_seeds_on_board(self):
        return sum(self.get_total_seeds_in_hole(i) for i in range(16))
    
    def parse_move(self, move_str):
        move_str = move_str.strip()
        if 'TB' in move_str:
            return int(move_str.replace('TB', '')), 'TB'
        elif 'TR' in move_str:
            return int(move_str.replace('TR', '')), 'TR'
        elif 'R' in move_str:
            return int(move_str.replace('R', '')), 'R'
        elif 'B' in move_str:
            return int(move_str.replace('B', '')), 'B'
        else:
            raise ValueError(f"Format invalide: {move_str}")
    
    def play_move(self, player, move_str):
        self.move_count += 1
        hole, color_type = self.parse_move(move_str)
        hole_idx = hole - 1
        
        # Vérification du joueur
        if player == 'A' and hole % 2 == 0:
            return False, f"Joueur A ne peut pas jouer le trou {hole}"
        if player == 'B' and hole % 2 == 1:
            return False, f"Joueur B ne peut pas jouer le trou {hole}"
        
        # Collecter les graines
        seeds_list = []
        
        if color_type == 'R':
            count = self.holes[hole_idx]['R']
            if count == 0:
                return False, f"Pas de graines rouges dans le trou {hole}"
            seeds_list = [('R', 'R')] * count
            self.holes[hole_idx]['R'] = 0
            behavior = 'R'
            
        elif color_type == 'B':
            count = self.holes[hole_idx]['B']
            if count == 0:
                return False, f"Pas de graines bleues dans le trou {hole}"
            seeds_list = [('B', 'B')] * count
            self.holes[hole_idx]['B'] = 0
            behavior = 'B'
            
        elif color_type == 'TR':
            t_count = self.holes[hole_idx]['T']
            r_count = self.holes[hole_idx]['R']
            if t_count + r_count == 0:
                return False, f"Pas de graines T+R dans le trou {hole}"
            seeds_list = [('T', 'R')] * t_count + [('R', 'R')] * r_count
            self.holes[hole_idx]['T'] = 0
            self.holes[hole_idx]['R'] = 0
            behavior = 'R'
            
        elif color_type == 'TB':
            t_count = self.holes[hole_idx]['T']
            b_count = self.holes[hole_idx]['B']
            if t_count + b_count == 0:
                return False, f"Pas de graines T+B dans le trou {hole}"
            seeds_list = [('T', 'B')] * t_count + [('B', 'B')] * b_count
            self.holes[hole_idx]['T'] = 0
            self.holes[hole_idx]['B'] = 0
            behavior = 'B'
        
        # Semer
        current_idx = hole_idx
        for seed_color, seed_behavior in seeds_list:
            while True:
                current_idx = (current_idx + 1) % 16
                if current_idx == hole_idx:
                    continue
                
                if behavior == 'B':
                    # Semer uniquement chez l'adversaire
                    if player == 'A' and (current_idx + 1) % 2 == 0:
                        break
                    elif player == 'B' and (current_idx + 1) % 2 == 1:
                        break
                else:
                    # Semer partout
                    break
            
            self.holes[current_idx][seed_color] += 1
            last_idx = current_idx
        
        # Capturer
        captured = self.capture_seeds(last_idx, player)
        return True, captured
    
    def capture_seeds(self, last_hole_idx, player):
        captured = 0
        current_idx = last_hole_idx
        
        while True:
            total = self.get_total_seeds_in_hole(current_idx)
            if total == 2 or total == 3:
                for color in ['R', 'B', 'T']:
                    captured += self.holes[current_idx][color]
                    self.holes[current_idx][color] = 0
                current_idx = (current_idx - 1) % 16
            else:
                break
        
        if player == 'A':
            self.score_A += captured
        else:
            self.score_B += captured
        
        return captured
    
    def is_game_over(self):
        return (self.score_A >= 49 or self.score_B >= 49 or 
                self.get_total_seeds_on_board() < 10 or 
                self.move_count >= 400)
    
    def afficher_plateau(self):
        print("État du plateau:")
        for i in range(16):
            print(f"Trou {i+1}: R={self.holes[i]['R']} B={self.holes[i]['B']} T={self.holes[i]['T']}")
        print(f"Score A: {self.score_A}, Score B: {self.score_B}\n")

    def validate_game(self, moves_text):
        for line in moves_text.strip().split('\n'):
            if '|' not in line or '->' not in line:
                continue
            
            parts = line.split('|')[1].split('->')
            player = parts[0].strip()
            move = parts[1].strip()
            
            if move.startswith('RESULT'):
                result = move.split()
                print(f"\nRésultat attendu: A={result[2]}, B={result[3]}")
                print(f"Résultat calculé: A={self.score_A}, B={self.score_B}")
                if self.score_A == int(result[2]) and self.score_B == int(result[3]):
                    print("✓ VALIDATION RÉUSSIE")
                else:
                    print("✗ DIVERGENCE")
                return
            
            valid, info = self.play_move(player, move)
            if not valid:
                print(f"Coup {self.move_count}: ERREUR - {info}")
                return
            
            print(f"Coup {self.move_count}: {player}->{move} | Capturé: {info} | A={self.score_A}, B={self.score_B}")
            # on affiche l'état du plateau si besoin
            self.afficher_plateau()

if __name__ == "__main__":
    import sys
    if len(sys.argv) != 2:
        print("Usage: python validator.py <moves_file>")
        sys.exit(1)
    
    moves_file = sys.argv[1]
    with open(moves_file, 'r') as f:
        moves_text = f.read()
    
    validator = GameValidator()
    validator.validate_game(moves_text)