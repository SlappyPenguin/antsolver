#include <bits/stdc++.h>
#include "../../include/random.h"
#include "../../include/solver.h"
#include "../../include/rank.h"
#include "../../include/convert.h"
using namespace std;

constexpr int NUM_GAMES = 100000;
constexpr int NUM_THREADS = 12;
const arr<str, NUM_STREETS> CLUSTERS_FILE = {
    "../data/preflop_clusters.bin", "../data/flop_clusters.bin",
    "../data/turn_clusters.bin", "../data/river_clusters.bin", 
};
const str GAMESTATES_FILE = "../data/gamestates.bin";

arr<Map<short>, NUM_STREETS> clusters;
void init_clusters() {
    for (int street = 0; street < NUM_STREETS; street++) {
        ifstream file(CLUSTERS_FILE[street], ios::binary);
        Map<short>& cluster = clusters[street];
        lint size = NUM_SETS[street];
        cluster.keys.resize(size), cluster.values.resize(size);
        read_range(file, cluster.keys), read_range(file, cluster.values);
    }
}

vec<Gamestate> gamestate(NUM_GAMES);
void rollout(int thread_id) {
    int num_per_thread = NUM_GAMES / NUM_THREADS;
    int first_game = thread_id * num_per_thread;
    int last_game = (thread_id == NUM_THREADS - 1) ? NUM_GAMES - 1 : (thread_id + 1) * num_per_thread - 1;

    vec<int> deck(NUM_CARDS);
    iota(deck.begin(), deck.end(), 0);
    arr<arr<int, NUM_FINAL_CARDS>, 2> cards;

    for (int game = first_game; game <= last_game; game++) {
        partial_shuffle(deck, 2 * NUM_HOLE_CARDS + NUM_PUBLIC_CARDS);
        for (int player : {0, 1}) {
            for (int i = 0; i < NUM_HOLE_CARDS; i++) 
                cards[player][i] = deck[NUM_HOLE_CARDS * player + i];
            for (int i = 0; i < NUM_PUBLIC_CARDS; i++)
                cards[player][NUM_HOLE_CARDS + i] = deck[2 * NUM_HOLE_CARDS + i];    
        }

        arr<int, 2> rank = {get_rank(cards[0]), get_rank(cards[1])};
        Winner& winner = gamestate[game].winner;
        if (rank[0] > rank[1]) winner = Winner::Zero;
        else if (rank[1] > rank[0]) winner = Winner::One;
        else winner = Winner::Draw;

        for (int player : {0, 1}) {
            cards[player] = convert(cards[player], (int) Street::River);
            for (int street = 0; street < NUM_STREETS; street++) {
                lint id = get_id(cards[player], street);
                int cluster = clusters[street].at(id);
                gamestate[game].cluster[player][street] = cluster;
            }
        }
    }
}

void print_gamestate() {
    ofstream file(GAMESTATES_FILE, ios::binary);
    for (int i = 0; i < NUM_GAMES; i++) {
        Gamestate game = gamestate[i];
        write(file, game.winner);
        for (int player : {0, 1})
            for (int street = 0; street < NUM_STREETS; street++)
                write(file, game.cluster[player][street]);
    }
}

int main() {
    init_ranks(), init_clusters();

    cout << "Generating for... ";
    auto start_time = chrono::high_resolution_clock::now();

    vec<thread> threads;
    for (int thread_id = 0; thread_id < NUM_THREADS; thread_id++) 
        threads.emplace_back(rollout, thread_id);
    for (thread& thread : threads)
        thread.join();
    
    print_gamestate();

    auto end_time = chrono::high_resolution_clock::now();
    chrono::duration<doub> elapsed_time = end_time - start_time;
    cout << fixed << setprecision(3);
    cout << elapsed_time.count() << " seconds" << '\n';
}


