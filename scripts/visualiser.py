# Align rectangles with top not bottom

import os
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle
from matplotlib.patches import Patch

FIGURE_SIZE = 4.8
NUM_RANKS = 13
MATRIX_RATIO = 1 / 1.33
RANKS = ["2", "3", "4", "5", "6", "7", "8", "9", "T", "J", "Q", "K", "A"]
ACTION_COLOR = {
    "Fold": "#1f77b4",
    "Check-Call": "#2ca02c",
    "25%-Pot": "#ff9999",
    "50%-Pot": "#ff6666",
    "75%-Pot": "#ff3333",
    "100%-Pot": "#cc0000",
    "200%-Pot": "#990000",
    "400%-Pot": "#660000",
    "All-In": "#660000"
}

def read_file():
    file_name = None
    while True:
        name = input("> Input file name (__.txt): ")
        file_name = "../data/" + name + ".txt"
        if os.path.isfile(file_name):
            break
        print("> File doesn't exist in data/, try again")

    with open(file_name, "r") as file:
        num_actions = int(file.readline().strip())
        actions = file.readline().strip().split()
        strategies, fills = [], []
        for line in file:
            line = line.strip()
            if not line:
                continue
            row = list(map(float, line.split()))
            fills.append(row[0])
            strategies.append(row[1:])
    return num_actions, actions, fills, strategies

# (row, col) coordinate + hand
def get_order():
    order = []
    for rank1 in range(NUM_RANKS):
        for rank2 in range(rank1, NUM_RANKS):
            larger = NUM_RANKS - 1 - rank1
            smaller = NUM_RANKS - 1 - rank2
            if rank1 == rank2:
                order.append((smaller, larger, f"{RANKS[rank2]}{RANKS[rank1]}"))
            else:
                order.append((smaller, larger, f"{RANKS[rank2]}{RANKS[rank1]}s"))
                order.append((larger, smaller, f"{RANKS[rank2]}{RANKS[rank1]}o"))
    return order

# converts to (x, y) coordinate + hand + fills + strat
def get_matrix(fills, strategies, num_actions):
    order = get_order()
    matrix = np.zeros((NUM_RANKS, NUM_RANKS, num_actions + 2), dtype=object)
    
    for i, strat in enumerate(strategies):
        row, col, hand = order[i]
        y = NUM_RANKS - 1 - row
        x = col
        matrix[x, y, 0] = hand 
        matrix[x, y, 1] = fills[i]
        matrix[x, y, 2:] = strat 
    return matrix

def plot_matrix(matrix, actions, num_actions):
    figure, axes = plt.subplots(figsize=(FIGURE_SIZE, FIGURE_SIZE))
    
    for x in range(NUM_RANKS):
        for y in range(NUM_RANKS):
            hand = matrix[x, y, 0]
            fill = matrix[x, y, 1]
            strat = matrix[x, y, 2:]

            shift = 0
            # Reversed because more convenient to fill left to right
            for k in range(num_actions - 1, -1, -1):
                prob = strat[k]
                rect = Rectangle((x + shift, y), prob, fill, facecolor=ACTION_COLOR[actions[k]], edgecolor="black")
                axes.add_patch(rect)
                shift += prob

            axes.text(
                x + 0.05,
                y + 0.05,
                hand,
                ha = "left",
                va = "top",
                fontsize = 13.5,
                color = "white",
                fontname = "Arial"
            )
    
    figure.patch.set_facecolor("white")
    axes.patch.set_facecolor("#333333")
    axes.set_aspect(MATRIX_RATIO)
    axes.set_xlim(0, NUM_RANKS)
    axes.set_ylim(0, NUM_RANKS)
    axes.set_xticks([])
    axes.set_yticks([])
    axes.set_title("Antsolver Strategy", fontweight = "bold", fontname = "Arial", fontsize = 16)
    
    legend = [Patch(facecolor=ACTION_COLOR[i], label=i) for i in actions]
    axes.legend(handles=legend, bbox_to_anchor=(1.05, 1), loc="upper left", prop={"family": "Arial"})
    
    plt.tight_layout()
    plt.show()

if __name__ == "__main__":
    num_actions, actions, fills, strategies = read_file()
    matrix = get_matrix(fills, strategies, num_actions)
    plot_matrix(matrix, actions, num_actions)