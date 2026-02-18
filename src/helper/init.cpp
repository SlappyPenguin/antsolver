#include <bits/stdc++.h>
#include "../../include/solver.h"
using namespace std;

void init_clusters(arr<Map<short>, NUM_STREETS>& clusters, const arr<str, NUM_STREETS>& clusters_file) {
    for (int street = 0; street < NUM_STREETS; street++) {
        ifstream file(clusters_file[street], ios::binary);
        Map<short>& cluster = clusters[street];
        lint size = NUM_SETS[street];
        cluster.keys.resize(size), cluster.values.resize(size);
        read_range(file, cluster.keys), read_range(file, cluster.values);
    }
}

void init_betstate(arr<Betstate, NUM_BETSTATES>& betstate, str betstates_file) {
    ifstream file(betstates_file, ios::binary);
    for (int i = 0; i < NUM_BETSTATES; i++) {
        Betstate& bet = betstate[i];
        read(file, bet.player), read(file, bet.street);    
        int num_children; read(file, num_children);
        for (int j = 0; j < num_children; j++) {
            Betstate::Child child;
            read(file, child.action), read(file, child.bet_id);
            bet.children.push_back(child);
        }
        read(file, bet.payoff.type), read(file, bet.payoff.amount);
    } 
}

void init_blueprint(
    const arr<Betstate, NUM_BETSTATES>& betstate,                 
    arr<arr<arr<arr<vec<Blueprint>, MAX_BUCKET_SIZE>, NUM_STREETS>, 2>, NUM_BIASES>& blueprint,
    str blueprint_file
) {
    ifstream file(blueprint_file, ios::binary);
    int bias = (int) Bias::Base;
    for (int i : {0, 1}) {
        for (int j = 0; j < NUM_STREETS; j++) {
            int bucket = get_non_leaf_bucket(i, j);
            int bucket_size = BUCKET_SIZE[bucket];
            int num_clusters = NUM_CLUSTERS[j];
            for (int k = 0; k < bucket_size; k++) {
                int bet_id = CUM_BUCKET_SIZE[bucket] + k;
                const Betstate& bet = betstate[bet_id];
                blueprint[bias][i][j][k].resize(num_clusters);
                for (int l = 0; l < num_clusters; l++) {
                    Blueprint& blue = blueprint[bias][i][j][k][l];
                    blue.num_actions = bet.children.size();
                    for (int m = 0; m < blue.num_actions; m++)
                        read(file, blue.strat[m]);
                }
            }
        }
    }
}

static void dfs_outside_limit(
    const arr<Betstate, NUM_BETSTATES>& betstate, 
    arr<arr<arr<arr<vec<Blueprint>, MAX_BUCKET_SIZE>, NUM_STREETS>, 2>, NUM_BIASES>& blueprint,
    int bet_id
) {
    const Betstate& bet = betstate[bet_id];
    const Betstate::Payoff& payoff = bet.payoff;
    if (payoff.type != PayoffType::NonLeaf) return;

    int player = bet.player, street = bet.street;
    int info_id = get_info_id(player, street, bet_id), num_clusters = NUM_CLUSTERS[street];

    for (int bias = (int) Bias::Base + 1; bias < NUM_BIASES; bias++) {
        blueprint[bias][player][street][info_id].resize(num_clusters);
        for (int cluster = 0; cluster < num_clusters; cluster++) {
            const Blueprint& base_blue = blueprint[(int) Bias::Base][player][street][info_id][cluster];
            Blueprint& blue = blueprint[bias][player][street][info_id][cluster];
            blue.init_from_blueprint(base_blue, bet, (Bias) bias, BIAS_MULTIPLIER);
        } 
    }
    for (const Betstate::Child& child : bet.children)    
        dfs_outside_limit(betstate, blueprint, child.bet_id);
}
static void dfs_inside_limit(
    const arr<Betstate, NUM_BETSTATES>& betstate, 
    arr<arr<arr<arr<vec<Blueprint>, MAX_BUCKET_SIZE>, NUM_STREETS>, 2>, NUM_BIASES>& blueprint,
    arr<arr<vec<Infoset>, MAX_BUCKET_SIZE>, 2>& infoset,
    arr<arr<vec<Infoset>, MAX_BUCKET_SIZE>, 2>& frontier,
    int bet_id, int start_street
) {
    const Betstate& bet = betstate[bet_id];
    const Betstate::Payoff& payoff = bet.payoff;
    if (payoff.type != PayoffType::NonLeaf) return;

    int player = bet.player, street = bet.street;
    int info_id = get_info_id(player, street, bet_id), num_clusters = NUM_CLUSTERS[street];
    if (street != start_street) {
        for (int decider : {0, 1}) {
            frontier[decider][info_id].resize(num_clusters);
            for (int cluster = 0; cluster < num_clusters; cluster++) 
                frontier[decider][info_id][cluster].num_actions = NUM_BIASES;
        }
        dfs_outside_limit(betstate, blueprint, bet_id);
        return;
    }
    
    infoset[player][info_id].resize(num_clusters);
    for (int cluster = 0; cluster < num_clusters; cluster++) 
        infoset[player][info_id][cluster].num_actions = bet.children.size();
    for (const Betstate::Child& child : bet.children)    
        dfs_inside_limit(betstate, blueprint, infoset, frontier, child.bet_id, start_street);
}
void init_search(
    const arr<Betstate, NUM_BETSTATES>& betstate, 
    arr<arr<arr<arr<vec<Blueprint>, MAX_BUCKET_SIZE>, NUM_STREETS>, 2>, NUM_BIASES>& blueprint,
    arr<arr<vec<Infoset>, MAX_BUCKET_SIZE>, 2>& infoset,
    arr<arr<vec<Infoset>, MAX_BUCKET_SIZE>, 2>& frontier,
    int bet_id
) {
    for (int player : {0, 1})   
        frontier[player].fill({}), infoset[player].fill({});
    int street = betstate[bet_id].street;
    dfs_inside_limit(betstate, blueprint, infoset, frontier, bet_id, street);
}




