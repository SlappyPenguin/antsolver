/*
Generates random games for MCCFR to train on. In future, I'd like the MCCFR to do chance branching while it's 
training (i.e. every time it gets to a new street, generate a card during training), but for now, I make every 
iteration use the same random card rollouts.
*/

#include <bits/stdc++.h>
#include "../../include/random.h"
#include "../../include/solver.h"
#include "../../include/rank.h"
#include "../../include/convert.h"
#include "../../include/init.h"
using namespace std;

constexpr int NUM_GAMES = 20000000;
constexpr int NUM_THREADS = 12;
const str GAMESTATES_FILE = "../data/gamestates.bin";

arr<Map<short>, NUM_STREETS> clusters;
vec<Gamestate> gamestate(NUM_GAMES);
void rollout(int thread_id) {
    pair<int, int> range = get_thread_range(NUM_GAMES, NUM_THREADS, thread_id);
    vec<int> deck(NUM_CARDS);
    iota(deck.begin(), deck.end(), 0);
    arr<arr<int, NUM_FINAL_CARDS>, 2> cards;

    for (int game = range.first; game <= range.second; game++) {
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
    init_ranks(), init_clusters(clusters);

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


