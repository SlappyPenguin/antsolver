/*
Finds the hand strength of every river card set. The hand strength of a set is its expected value against a 
uniformly random distribution of opponent hands, where a win is +1, a draw 0.5, and a loss 0.
*/

#include <bits/stdc++.h>
#include "../../include/solver.h"
#include "../../include/rank.h"
using namespace std;

const str INPUT_FILE = "../data/river_sets.bin";
const str OUTPUT_FILE = "../data/hand_strengths.bin";

float get_hand_strength(arr<int, NUM_FINAL_CARDS> cards) {
    int my_rank = get_rank(cards);
    lint used_mask = 0;
    for (int card : cards) 
        used_mask |= (1ll << card);

    float sum = 0;
    int count = 0;
    for (int i = 0; i < NUM_CARDS; i++) {
        if (used_mask & (1ll << i)) continue;
        for (int j = i + 1; j < NUM_CARDS; j++) {
            if (used_mask & (1ll << j)) continue;
            count++;
            cards[0] = i, cards[1] = j;

            int opp_rank = get_rank(cards);
            if (my_rank > opp_rank) sum += 1;
            else if (my_rank == opp_rank) sum += 0.5; 
        }
    }
    float ans = sum / count;
    return ans;
}

vec<float> strength;
int main() {
    init_ranks();

    cout << "Generating for... ";
    auto start_time = chrono::high_resolution_clock::now();

    ifstream input_file(INPUT_FILE, ios::binary);
    ofstream output_file(OUTPUT_FILE, ios::binary);

    lint num_sets = NUM_SETS[(int) Street::River];
    strength.resize(num_sets);
    for (lint i = 0; i < num_sets; i++) {
        lint id; read(input_file, id);
        arr<int, NUM_FINAL_CARDS> cards = get_cards(id, (int) Street::River);
        strength[i] = get_hand_strength(cards);
        write(output_file, id);
    }
    for (lint i = 0; i < num_sets; i++) 
        write(output_file, strength[i]);
    
    auto end_time = chrono::high_resolution_clock::now();
    chrono::duration<doub> elapsed_time = end_time - start_time;
    cout << fixed << setprecision(3);
    cout << elapsed_time.count() << " seconds" << '\n';
}