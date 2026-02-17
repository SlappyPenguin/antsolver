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
    for (int i = 0; i < num_sets; i++) {
        short cluster = i;
        write(clusters_file, cluster);
    }
}

void cluster_river() {
    ifstream strengths_file(RIVER_STRENGTHS_FILE, ios::binary);
    ofstream clusters_file(RIVER_CLUSTERS_FILE, ios::binary);
    int street = (int) Street::River;
    lint num_sets = NUM_SETS[street];
    vec<lint> ids(num_sets);
    vec<float> strengths(num_sets);
    read_range(strengths_file, ids), read_range(strengths_file, strengths);
    
    arr<lint, NUM_SECTIONS> size;
    for (float strength : strengths)
        size[get_section(strength)]++;

    arr<vec<pair<int, lint>>, NUM_SECTIONS> sections;
    int cluster = 0, section = 0;
    lint remaining = get_cluster_size(cluster);
    while (cluster < NUM_CLUSTERS[street]) {
        lint to_remove = min(remaining, size[section]);
        if (to_remove > 0) sections[section].push_back({cluster, to_remove});
        remaining -= to_remove, size[section] -= to_remove;

        if (size[section] == 0) section++;
        if (remaining == 0) {
            cluster++;
            remaining = get_cluster_size(cluster);
        }
    }

    write_range(clusters_file, ids);
    for (lint i = 0; i < num_sets; i++) {
        int section = get_section(strengths[i]);
        pair<int, lint>& cluster_group = sections[section].back();
        short cluster = cluster_group.first;
        write(clusters_file, cluster);

        cluster_group.second--;
        if (cluster_group.second == 0)
            sections[section].pop_back();
    }
}

int main() {
    cout << "Generating for... ";
    auto start_time = chrono::high_resolution_clock::now();

    cluster_preflop();
    cluster_river();

    auto end_time = chrono::high_resolution_clock::now();
    chrono::duration<doub> elapsed_time = end_time - start_time;
    cout << fixed << setprecision(3);
    cout << elapsed_time.count() << " seconds" << '\n';
}

