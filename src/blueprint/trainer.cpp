/*
Runs MCCFR on the abstracted game tree until an approximate equilibrium strategy is generated. 

NOTE: This code is not thread-safe, i.e. race conditions can happen. From testing it seems this is faster and has 
no noticable effect on correctness.
*/

#include <bits/stdc++.h>
#include "../../include/solver.h"
#include "../../include/init.h"
using namespace std;

constexpr int NUM_GAMES = 20000000;
constexpr int NUM_THREADS = 12;
const str GAMESTATES_FILE = "../data/gamestates.bin";
const str INFOSETS_FILE = "../data/infosets.bin";

arr<Betstate, NUM_BETSTATES> betstate;
vec<Gamestate> gamestate(NUM_GAMES);
arr<arr<arr<vec<Infoset>, MAX_BUCKET_SIZE>, NUM_STREETS>, 2> infoset;
void init_gamestate() {
    ifstream file(GAMESTATES_FILE, ios::binary);
    for (int i = 0; i < NUM_GAMES; i++) {
        Gamestate& game = gamestate[i];
        read(file, game.winner);
        for (int player : {0, 1})
            for (int street = 0; street < NUM_STREETS; street++)
                read(file, game.cluster[player][street]);
    }
}

inline float get_leaf_value(const Betstate::Payoff& payoff, const Gamestate& game, int learner) {
    Winner winner = game.winner;
    int payoff_for_0 = 0; 
    if (payoff.type == PayoffType::Fixed || winner == Winner::Zero) payoff_for_0 = payoff.amount;
    else if (winner == Winner::One) payoff_for_0 = -payoff.amount;
    return (learner == 0) ? payoff_for_0 : -payoff_for_0;
}
float get_value(const Gamestate& game, int bet_id, int learner) {
    const Betstate& bet = betstate[bet_id];
    const Betstate::Payoff& payoff = bet.payoff;
    if (payoff.type != PayoffType::NonLeaf) 
        return get_leaf_value(payoff, game, learner);

    int player = bet.player, street = bet.street;
    int info_id = get_info_id(player, street, bet_id), cluster = game.cluster[player][street];
    Infoset& info = infoset[player][street][info_id][cluster];
    info.upd_strat();
    
    float value = 0;
    if (player == learner) {
        arr<float, MAX_NUM_ACTIONS> action_value;
        for (int i = 0; i < info.num_actions; i++) {
            int new_bet_id = bet.children[i].bet_id;
            action_value[i] = get_value(game, new_bet_id, learner);
            value += action_value[i] * info.strat[i];
        }
        for (int i = 0; i < info.num_actions; i++)
            info.cum_regret[i] += action_value[i] - value;
    } else {
        int action_id = info.get_sampled_action();
        int new_bet_id = bet.children[action_id].bet_id;
        value = get_value(game, new_bet_id, learner);
        for (int i = 0; i < info.num_actions; i++) 
            info.cum_strat[i] += info.strat[i];
    }
    return value;
}
void train(int thread_id) {
    pair<int, int> range = get_thread_range(NUM_GAMES, NUM_THREADS, thread_id);
    for (int game = range.first; game <= range.second; game++) {
        int learner = game % 2;
        get_value(gamestate[game], 0, learner);
    }
}

void print_infoset() {
    ofstream file(INFOSETS_FILE, ios::binary);
    for (int i : {0, 1}) {
        for (int j = 0; j < NUM_STREETS; j++) {
            int bucket = get_non_leaf_bucket(i, j);
            int bucket_size = BUCKET_SIZE[bucket];
            int num_clusters = NUM_CLUSTERS[j];
            for (int k = 0; k < bucket_size; k++) {
                for (int l = 0; l < num_clusters; l++) {
                    Infoset& info = infoset[i][j][k][l];
                    for (int m = 0; m < info.num_actions; m++)
                        write(file, info.cum_regret[m]);
                    for (int m = 0; m < info.num_actions; m++)
                        write(file, info.cum_strat[m]);
                }
            }
        }
    }
}

int main() {
    init_betstate(betstate), init_gamestate(), init_infoset(betstate, infoset);

    cout << "Training for... ";
    auto start_time = chrono::high_resolution_clock::now();

    vec<thread> threads;
    for (int thread_id = 0; thread_id < NUM_THREADS; thread_id++) 
        threads.emplace_back(train, thread_id);
    for (thread& thread : threads)
        thread.join();
    
    print_infoset();

    auto end_time = chrono::high_resolution_clock::now();
    chrono::duration<doub> elapsed_time = end_time - start_time;
    cout << fixed << setprecision(3);
    cout << elapsed_time.count() << " seconds" << '\n';
}

