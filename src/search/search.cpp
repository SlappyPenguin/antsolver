#include <bits/stdc++.h>
#include "../../include/solver.h"
#include "../../include/search.h"
#include "../../include/rank.h"
#include "../../include/convert.h"
#include "../../include/init.h"
#include "../../include/retrainer.h"
using namespace std;

constexpr int NUM_ROLLOUTS = 3;
constexpr int NUM_THREADS = 12;
constexpr arr<int, NUM_STREETS> NUM_GAMES = {0, 20000000, 30000000, 80000000};
constexpr int MAX_NUM_GAMES = []{
    return *max_element(NUM_GAMES.begin(), NUM_GAMES.end());
}();

static arr<arr<arr<doub, NUM_CARDS>, NUM_CARDS>, 2> reach_prob;
static void normalise_reach_prob() {
    for (int player : {0, 1}) {
        doub sum = 0;
        for (int i = 0; i < NUM_CARDS; i++)
            for (int j = i + 1; j < NUM_CARDS; j++)
                sum += reach_prob[player][i][j];
        for (int i = 0; i < NUM_CARDS; i++)
            for (int j = i + 1; j < NUM_CARDS; j++)
                reach_prob[player][i][j] /= sum;
    }
}
void init_reach_prob() {
    for (int player : {0, 1})
        for (int i = 0; i < NUM_CARDS; i++)
            for (int j = i + 1; j < NUM_CARDS; j++)
                reach_prob[player][i][j] = 1;
    normalise_reach_prob();
}
void update_reach_prob(
    const arr<bool, NUM_CARDS>& in_public, 
    arr<int, NUM_FINAL_CARDS> cards, 
    int bet_id, int action
) {
    const Betstate& bet = betstate[bet_id];
    int player = bet.player, street = bet.street, info_id = get_info_id(player, street, bet_id);
    for (int i = 0; i < NUM_CARDS; i++) {
        for (int j = i + 1; j < NUM_CARDS; j++) {
            if (in_public[i] || in_public[j]) {
                reach_prob[player][i][j] = 0;
                continue;
            }
            cards[0] = i, cards[1] = j;
            lint id = get_id(convert(cards, street), street);
            int cluster = clusters[street].at(id);
            
            bool at_preflop = (street == (int) Street::Preflop);
            Blueprint blue;
            if (!at_preflop) {
                const Infoset& info = infoset[player][info_id][cluster];
                blue.init_from_infoset(info);
            } else {
                blue = blueprint[(int) Bias::Base][player][street][info_id][cluster];
            }
            reach_prob[player][i][j] *= blue.strat[action];
        }
    }
    normalise_reach_prob();
}
float get_reach_prob(arr<int, NUM_HOLE_CARDS> hole_cards, int player) {
    return reach_prob[player][hole_cards[0]][hole_cards[1]];
}

static arr<arr<arr<arr<doub, NUM_CARDS>, NUM_CARDS>, NUM_CARDS>, NUM_CARDS> combined_prob;
static void normalise_combined_prob() {
    doub sum = 0;
    for (int i = 0; i < NUM_CARDS; i++)
        for (int j = i + 1; j < NUM_CARDS; j++)
            for (int k = 0; k < NUM_CARDS; k++)
                for (int l = k + 1; l < NUM_CARDS; l++)
                    sum += combined_prob[i][j][k][l];
    for (int i = 0; i < NUM_CARDS; i++)
        for (int j = i + 1; j < NUM_CARDS; j++)
            for (int k = 0; k < NUM_CARDS; k++)
                for (int l = k + 1; l < NUM_CARDS; l++)
                    combined_prob[i][j][k][l] /= sum;        
}
static void init_combined_prob(const arr<bool, NUM_CARDS>& in_public) {
    for (int i = 0; i < NUM_CARDS; i++) {
        for (int j = i + 1; j < NUM_CARDS; j++) {
            for (int k = 0; k < NUM_CARDS; k++) {
                for (int l = k + 1; l < NUM_CARDS; l++) {
                    bool is_shared = (k == i || k == j || l == i || l == j);
                    bool is_public = (in_public[i] || in_public[j] || in_public[k] || in_public[l]); 
                    combined_prob[i][j][k][l] = (is_shared || is_public) ? 0 : reach_prob[0][i][j] * reach_prob[1][k][l];
                }
            }
        }
    }
    normalise_combined_prob();
}

static vec<doub> random(MAX_NUM_GAMES);
static vec<Gamestate> gamestate(MAX_NUM_GAMES);
static vec<arr<arr<int, NUM_HOLE_CARDS>, 2>> hole_cards(MAX_NUM_GAMES);
static void init_gamestate(int bet_id) {
    int street = betstate[bet_id].street, num_games = NUM_GAMES[street];
    for (int i = 0; i < num_games; i++)
        random[i] = get_generator().get_random_float();
    sort(random.begin(), random.begin() + num_games);

    int next_index = 0;
    doub cum = 0;
    for (int i = 0; i < NUM_CARDS; i++) {
        for (int j = i + 1; j < NUM_CARDS; j++) {
            for (int k = 0; k < NUM_CARDS; k++) {
                for (int l = k + 1; l < NUM_CARDS; l++) {
                    if (combined_prob[i][j][k][l] == 0) continue;
                    cum += combined_prob[i][j][k][l];
                    while (next_index < num_games && cum >= random[next_index]) {
                        hole_cards[next_index] = {{{i, j}, {k, l}}};
                        next_index++;
                    }
                }
            }
        }
    }
    int last_index = next_index - 1;
    while (next_index < num_games) {
        hole_cards[next_index] = hole_cards[last_index];
        next_index++;
    }
}

static void set_gamestate(
    vec<int>& remaining_cards, 
    arr<arr<int, NUM_FINAL_CARDS>, 2> cards, 
    int bet_id, int index
) {
    const Betstate& bet = betstate[bet_id];
    int num_needed = NUM_FINAL_CARDS - CUM_STREET_SIZE[bet.street];
    partial_shuffle(remaining_cards, num_needed);
    for (int player : {0, 1}) 
        for (int i = 0; i < num_needed; i++)
            cards[player][CUM_STREET_SIZE[bet.street] + i] = remaining_cards[i];

    arr<int, 2> rank = {get_rank(cards[0]), get_rank(cards[1])};
    Winner& winner = gamestate[index].winner;
    if (rank[0] > rank[1]) winner = Winner::Zero;
    else if (rank[1] > rank[0]) winner = Winner::One;
    else winner = Winner::Draw;

    for (int player : {0, 1}) {
        cards[player] = convert(cards[player], (int) Street::River);                
        for (int street = bet.street; street < NUM_STREETS; street++) {
            lint id = get_id(cards[player], street);
            int cluster = clusters[street].at(id);
            gamestate[index].cluster[player][street] = cluster;
        }
    }
}
static void gen(const arr<int, NUM_FINAL_CARDS>& cards, vec<int> remaining_cards, int bet_id, int thread_id) {
    int street = betstate[bet_id].street;
    pair<int, int> range = get_thread_range(NUM_GAMES[street], NUM_THREADS, thread_id);
    arr<arr<int, NUM_FINAL_CARDS>, 2> player_cards = {cards, cards};
    for (int game = range.first; game <= range.second; game++) {
        for (int player : {0, 1}) {
            for (int i = 0; i < NUM_HOLE_CARDS; i++) {
                player_cards[player][i] = hole_cards[game][player][i];
                erase_value(remaining_cards, player_cards[player][i]);
            }
        }
        set_gamestate(remaining_cards, player_cards, bet_id, game);
        for (int player : {0, 1}) 
            for (int i = 0; i < NUM_HOLE_CARDS; i++) 
                remaining_cards.push_back(player_cards[player][i]);
    }
}
static void gen_gamestate(const arr<int, NUM_FINAL_CARDS>& cards, vec<int>& remaining_cards, int bet_id) {
    vec<thread> threads;
    for (int i = 0; i < NUM_THREADS; i++)
        threads.emplace_back(gen, cards, remaining_cards, bet_id, i);
    for (thread& thread : threads)
        thread.join();

    int street = betstate[bet_id].street;
    shuffle(gamestate.begin(), gamestate.begin() + NUM_GAMES[street], get_slow_generator());
}

static void train(int bet_id, int thread_id) {
    int street = betstate[bet_id].street;
    pair<int, int> range = get_thread_range(NUM_GAMES[street], NUM_THREADS, thread_id);
    for (int game = range.first; game <= range.second; game++) {
        int learner = game % 2;
        get_value(gamestate[game], bet_id, learner, street, NUM_ROLLOUTS);
    }
}
static void train_infoset(int bet_id) {
    vec<thread> threads;
    for (int i = 0; i < NUM_THREADS; i++)
        threads.emplace_back(train, bet_id, i);
    for (thread& thread : threads)
        thread.join();
}

void search(
    const arr<int, NUM_FINAL_CARDS>& cards, 
    const arr<bool, NUM_CARDS>& in_public, 
    vec<int> remaining_cards, 
    int bet_id
) {
    cout << "> Searching for... ";
    auto start_time = chrono::high_resolution_clock::now();
    
    init_search(betstate, blueprint, infoset, frontier, bet_id); 
    init_combined_prob(in_public);
    init_gamestate(bet_id);
    gen_gamestate(cards, remaining_cards, bet_id);
    train_infoset(bet_id);

    auto end_time = chrono::high_resolution_clock::now();
    chrono::duration<doub> elapsed_time = end_time - start_time;
    cout << round(elapsed_time.count()) << " seconds" << endl;
}
