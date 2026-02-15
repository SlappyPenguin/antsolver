#include <bits/stdc++.h>
#include "../../include/solver.h"
using namespace std;

const str STRENGTHS_FILE = "../data/hand_strengths.bin";
const arr<str, NUM_STREETS> SETS_FILE = {
    "", 
    "../data/flop_sets.bin", 
    "../data/turn_sets.bin", 
    ""
};
const arr<str, NUM_STREETS> DISTRIBUTIONS_FILE = {
    "", 
    "../data/flop_distributions.bin", 
    "../data/turn_distributions.bin", 
    ""
};

Map<float> strength;
void init_maps() {
    ifstream file(STRENGTHS_FILE, ios::binary);
    lint size = NUM_SETS[(int) Street::River];
    strength.keys.resize(size), strength.values.resize(size);
    read_range(file, strength.keys), read_range(file, strength.values);
}

int main() {
    init_maps();

    cout << "Generating for... ";
    auto start_time = chrono::high_resolution_clock::now();

    


    auto end_time = chrono::high_resolution_clock::now();
    chrono::duration<doub> elapsed_time = end_time - start_time;
    cout << fixed << setprecision(3);
    cout << elapsed_time.count() << " seconds" << '\n';

}