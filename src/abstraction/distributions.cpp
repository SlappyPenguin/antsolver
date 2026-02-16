#include <bits/stdc++.h>
#include "../../include/solver.h"
#include "../../include/convert.h"
using namespace std;

const str STRENGTHS_FILE = "../data/hand_strengths.bin";
const arr<str, NUM_STREETS> SETS_FILE = {
    "", "../data/flop_sets.bin",
    "../data/turn_sets.bin", ""
};
const arr<str, NUM_STREETS> DISTRIBUTIONS_FILE = {
    "", "../data/flop_distributions.bin",
    "../data/turn_distributions.bin", ""
};

Map<float> strength;
void init_strength() {
    ifstream file(STRENGTHS_FILE, ios::binary);
    lint size = NUM_SETS[(int) Street::River];
    strength.keys.resize(size), strength.values.resize(size);
    read_range(file, strength.keys), read_range(file, strength.values);
}

arr<float, NUM_INTERVALS> get_distribution(arr<int, NUM_FINAL_CARDS> cards, int street) {
    int size_to_street = CUM_STREET_SIZE[street];
    arr<bool, NUM_CARDS> used = {};
    for (int i = 0; i < size_to_street; i++)
        used[cards[i]] = true;

    arr<float, NUM_INTERVALS> distribution = {};
    int count = 0;
    if (street == (int) Street::Flop) {
        for (int i = 0; i < NUM_CARDS; i++) {
            if (used[i]) continue;
            for (int j = 0; j < NUM_CARDS; j++) {
                if (used[j]) continue;
                if (j == i) continue;
                count++;

                cards[size_to_street] = i, cards[size_to_street + 1] = j;
                lint converted_id = get_id(convert(cards, street), street);
                float hand_strength = strength.at(converted_id);
                distribution[get_interval_id(hand_strength)]++;
            }
        }
    } else if (street == (int) Street::Turn) {
        for (int i = 0; i < NUM_CARDS; i++) {
            if (used[i]) continue;
            count++;

            cards[size_to_street] = i;
            lint converted_id = get_id(convert(cards, street), street);
            float hand_strength = strength.at(converted_id);
            distribution[get_interval_id(hand_strength)]++;
        }
    } else assert(false);

    for (int j = 0; j < NUM_INTERVALS; j++)
        distribution[j] /= count;
    return distribution;
}

int main() {
    init_strength();

    cout << "Generating for... ";
    auto start_time = chrono::high_resolution_clock::now();

    for (int street : {(int) Street::Flop, (int) Street::Turn}) {
        ifstream input_file(SETS_FILE[street], ios::binary);
        ofstream output_file(DISTRIBUTIONS_FILE[street], ios::binary);

        int num_sets = NUM_SETS[street];
        vec<lint> ids(num_sets);
        read_range(input_file, ids);
        write_range(output_file, ids);
        for (int i = 0; i < 50; i++) {
            lint id = ids[i];
            arr<int, NUM_FINAL_CARDS> cards = get_cards(id, street);
            arr<float, NUM_INTERVALS> distribution = get_distribution(cards, street);
            for (float prob : distribution)
                write(output_file, prob);
        }
    }

    auto end_time = chrono::high_resolution_clock::now();
    chrono::duration<doub> elapsed_time = end_time - start_time;
    cout << fixed << setprecision(3);
    cout << elapsed_time.count() << " seconds" << '\n';
}