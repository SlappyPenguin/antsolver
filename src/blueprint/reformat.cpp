/*
Reformats the layout of infosets.bin (which includes cumulative regret and strategy values) into a pure blueprint
strategy.
*/

#include <bits/stdc++.h>
#include "../../include/solver.h"
#include "../../include/init.h"
using namespace std;

const str BETSTATES_FILE = "../data/betstates.bin";
const str INFOSETS_FILE = "../data/infosets.bin";
const str BLUEPRINT_FILE = "../data/blueprint.bin";

arr<Betstate, NUM_BETSTATES> betstate;
arr<arr<arr<vec<Infoset>, MAX_BUCKET_SIZE>, NUM_STREETS>, 2> infoset;
void print_blueprint() {
    ofstream file(BLUEPRINT_FILE, ios::binary);
    for (int i : {0, 1}) {
        for (int j = 0; j < NUM_STREETS; j++) {
            int bucket = get_non_leaf_bucket(i, j);
            int bucket_size = BUCKET_SIZE[bucket];
            int num_clusters = NUM_CLUSTERS[j];
            for (int k = 0; k < bucket_size; k++) {
                for (int l = 0; l < num_clusters; l++) {
                    const Infoset& info = infoset[i][j][k][l];
                    Blueprint blue;
                    blue.init_from_infoset(info);
                    for (int m = 0; m < blue.num_actions; m++)
                        write(file, blue.strat[m]);
                }
            }
        }
    }
}

int main() {
    init_betstate(betstate, BETSTATES_FILE), init_infoset(betstate, infoset, INFOSETS_FILE);
    
    cout << "Reformatting for... ";
    auto start_time = chrono::high_resolution_clock::now();

    print_blueprint();

    auto end_time = chrono::high_resolution_clock::now();
    chrono::duration<doub> elapsed_time = end_time - start_time;
    cout << fixed << setprecision(3);
    cout << elapsed_time.count() << " seconds" << '\n';
}

