import numpy as np
import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle
from matplotlib.patches import Patch

FIGURE_SIZE = 4.8
NUM_RANKS = 13
MATRIX_RATIO = 1 / 1.33
FILE_NAME = "../data/visualise.txt"
RANKS = ["A","K","Q","J","T","9","8","7","6","5","4","3","2"]
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
    with open(FILE_NAME, "r") as file:
        num_actions = int(file.readline().strip())
        actions = file.readline().strip().split()
        strategies = []
        for line in file:
            line = line.strip()
            if not line:
                continue
            row = list(map(float, line.split()))
            strategies.append(row)
    return num_actions, actions, strategies

def get_order():
    order = []
    for i in range(NUM_RANKS - 1, -1, -1):
        for j in range(i, -1, -1):
            if i != j:
                order.append((i, j))
            order.append((j, i))   
    return order

def get_matrix(strategies, num_actions):
    order = get_order()
    matrix = np.zeros((NUM_RANKS, NUM_RANKS, num_actions))
    
    for i, strat in enumerate(strategies):
        row, col = order[i]
        matrix[row, col, :] = strat
    return matrix

def plot_matrix(matrix, actions, num_actions):
    figure, axes = plt.subplots(figsize=(FIGURE_SIZE, FIGURE_SIZE))
    
    for i in range(NUM_RANKS):
        for j in range(NUM_RANKS):
            probs = matrix[i, j, :]
            left = 0
            # Reversed since it otherwise it fills right to left
            for k in range(num_actions - 1, -1, -1):
                prob = probs[k]
                rect = Rectangle((i + left, j), prob, 1, facecolor=ACTION_COLOR[actions[k]], edgecolor="black")
                axes.add_patch(rect)
                left += prob

            rank1 = RANKS[i]
            rank2 = RANKS[j]
            text = ""
            if i == j:
                text = f"{rank1}{rank2}"
            elif (i < j):
                text = f"{rank1}{rank2}o"
            else:
                text = f"{rank2}{rank1}s"

            axes.text(
                i + 0.05,
                j + 0.05,
                text,
                ha = "left",
                va = "top",
                fontsize = 13.5,
                color = "white",
                fontname = "Arial"
            )
    
    axes.set_aspect(MATRIX_RATIO)
    axes.set_xlim(0, NUM_RANKS)
    axes.set_ylim(0, NUM_RANKS)
    axes.set_xticks([])
    axes.set_yticks([])
    axes.set_title("Antsolver Strategy", fontweight = "bold", fontname = "Arial", fontsize = 16)
    plt.gca().invert_yaxis()
    
    legend = [Patch(facecolor=ACTION_COLOR[i], label=i) for i in actions]
    axes.legend(handles=legend, bbox_to_anchor=(1.05, 1), loc="upper left", prop={"family": "Arial"})
    
    plt.tight_layout()
    plt.show()

if __name__ == "__main__":
    num_actions, actions, strategies = read_file()
    matrix = get_matrix(strategies, num_actions)
    plot_matrix(matrix, actions, num_actions)