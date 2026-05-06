/*
No card abstraction is done on the preflop, i.e. there are 169 unique clusters corresponding to each card set.

On the river, sets are clustered by percentile hand strength into 1000 clusters.
*/

#include <chrono>
#include <iomanip>
#include <iostream>
#include "../../include/solver.h"
using namespace std;

constexpr int NUM_SECTIONS = 1000;
constexpr float SECTION_SIZE = []{
    return 1 / (float) NUM_SECTIONS;
}();
const str SETS_FILE = "../data/preflop_sets.bin";
const str STRENGTHS_FILE = "../data/hand_strengths.bin";
const arr<str, NUM_STREETS> CLUSTERS_FILE = {
    "../data/preflop_clusters.bin", "",
    "", "../data/river_clusters.bin"
};

void cluster_preflop() {
    int street = (int) Street::Preflop, num_sets = NUM_SETS[street];
    ifstream sets_file(SETS_FILE, ios::binary);
    ofstream clusters_file(CLUSTERS_FILE[street], ios::binary);
    vec<lint> ids(num_sets);
    read_range(sets_file, ids), write_range(clusters_file, ids);
    for (int i = 0; i < num_sets; i++) {
        short cluster = i;
        write(clusters_file, cluster);
    }
}

inline int get_section(float strength) {
    return min((int) (strength / SECTION_SIZE), NUM_SECTIONS - 1);
}
inline lint get_cluster_size(int cluster) {
    int street = (int) Street::River;
    lint non_last_size = NUM_SETS[street] / NUM_SECTIONS;
    lint last_size = NUM_SETS[street] - (NUM_SECTIONS - 1) * non_last_size;
    return (cluster == NUM_CLUSTERS[street] - 1) ? last_size : non_last_size;
}
void cluster_river() {
    int street = (int) Street::River;
    lint num_sets = NUM_SETS[street];
    ifstream strengths_file(STRENGTHS_FILE, ios::binary);
    ofstream clusters_file(CLUSTERS_FILE[street], ios::binary);
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

