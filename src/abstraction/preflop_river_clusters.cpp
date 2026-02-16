/*
No card abstraction is done on the preflop, i.e. there are 169 unique clusters corresponding to each card set.

On the river, sets are clustered by percentile hand strength. 
*/

#include <bits/stdc++.h>
#include "../../include/solver.h"
using namespace std;

const str PREFLOP_SETS_FILE = "../data/preflop_sets.bin";
const str RIVER_STRENGTHS_FILE = "../data/hand_strengths.bin";
const str PREFLOP_CLUSTERS_FILE = "../data/preflop_clusters.bin";
const str RIVER_CLUSTERS_FILE = "../data/river_clusters.bin";

void cluster_preflop() {
    ifstream sets_file(PREFLOP_SETS_FILE, ios::binary);
    ofstream clusters_file(PREFLOP_CLUSTERS_FILE, ios::binary);
    int num_sets = NUM_SETS[(int) Street::Preflop];
    vec<lint> ids(num_sets);
    read_range(sets_file, ids), write_range(clusters_file, ids);
    for (int i = 0; i < num_sets; i++)
        write(clusters_file, i);
}
void cluster_river() {
    ifstream strengths_file(RIVER_STRENGTHS_FILE, ios::binary);
    ofstream clusters_file(RIVER_CLUSTERS_FILE, ios::binary);
    

}

int main() {
    cluster_preflop();
    cluster_river();
}

