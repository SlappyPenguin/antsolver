#pragma once

#include <bits/stdc++.h>
#include "random.h"
using namespace std;
using lint = long long;
using doub = double;
using str = string;
template<typename T, size_t N> 
using arr = array<T, N>;
template<typename T> 
using vec = vector<T>; 

enum class Action : int {
    Fold, CheckCall,
    Bet25, Bet50, Bet75,
    Bet100, Bet200, Bet400, AllIn
};
enum class PayoffType : int {
    NonLeaf, Fixed, Unfixed
};
enum class Bias : int {
    Base, Folding, CheckCalling, Betting
};
enum class Street : int {
    Preflop, Flop, Turn, River
};
enum class Winner : int {
    Zero, One, Draw
};

constexpr int NUM_BETSTATES = 809309;
constexpr int MAX_NUM_ACTIONS = 9;
constexpr int NUM_BIASES = 4;
constexpr int NUM_STREETS = 4;
constexpr int NUM_CARDS = 52;
constexpr int NUM_FINAL_CARDS = 7;
constexpr int NUM_PUBLIC_CARDS = 5;
constexpr int NUM_HOLE_CARDS = 2;
constexpr int NUM_BUCKETS = 9;
constexpr int MAX_BUCKET_SIZE = 121538;
constexpr int NUM_SUITS = 4;
constexpr int NUM_RANKS = 13;
constexpr int SMALL_BLIND = 50;
constexpr int BIG_BLIND = 100;
constexpr int STACK = 20000;
constexpr int NUM_INTERVALS = 10;
constexpr float BIAS_MULTIPLIER = 10;
constexpr lint GIGABYTE = 1ll << 30;
constexpr arr<int, NUM_STREETS> NUM_CLUSTERS = {169, 1000, 1000, 1000};
constexpr arr<int, NUM_BUCKETS> BUCKET_SIZE = {30, 1396, 17896, 121538, 30, 1396, 17896, 121538, 527589};
constexpr arr<int, NUM_BUCKETS> CUM_BUCKET_SIZE = {0, 30, 1426, 19322, 140860, 140890, 142286, 160182, 281720};
constexpr arr<int, NUM_STREETS> STREET_SIZE = {2, 3, 1, 1};
constexpr arr<int, NUM_STREETS> CUM_STREET_SIZE = {2, 5, 6, 7};
constexpr arr<int, NUM_FINAL_CARDS> STREET_AT = {0, 0, 1, 1, 1, 2, 3};
constexpr arr<lint, NUM_STREETS> NUM_SETS = {169, 1286792, 55190538, 2428287420};
constexpr arr<float, MAX_NUM_ACTIONS> POT_PERCENT = {0, 0, 0.25, 0.50, 0.75, 1.00, 2.00, 4.00, 0};

const map<str, int> POSITION_VALUE = {
    {"Small-Blind", 0}, {"Big-Blind", 1}
};
const map<int, str> POSITION_NAME = {
    {0, "Small-Blind"}, {1, "Big-Blind"}
};
const map<Street, str> STREET_NAME = {
    {Street::Preflop, "preflop"}, {Street::Flop, "flop"}, 
    {Street::Turn, "turn"}, {Street::River, "river"}
};
const map<str, Street> STREET_VALUE = {
    {"preflop", Street::Preflop}, {"flop", Street::Flop}, 
    {"turn", Street::Turn}, {"river", Street::River}
};
const map<Action, str> ACTION_NAME = {
    {Action::Fold, "Fold"}, {Action::CheckCall, "Check-Call"}, {Action::Bet25, "25%-Pot"},
    {Action::Bet50, "50%-Pot"}, {Action::Bet75, "75%-Pot"}, {Action::Bet100, "100%-Pot"},
    {Action::Bet200, "200%-Pot"}, {Action::Bet400, "400%-Pot"}, {Action::AllIn, "All-In"}
};
const map<str, Action> ACTION_VALUE = {
    {"Fold", Action::Fold}, {"Check-Call", Action::CheckCall}, {"25%-Pot", Action::Bet25},
    {"50%-Pot", Action::Bet50}, {"75%-Pot", Action::Bet75}, {"100%-Pot", Action::Bet100},
    {"200%-Pot", Action::Bet200}, {"400%-Pot", Action::Bet400}, {"All-In", Action::AllIn}
};
const map<str, int> CARD_VALUE = {
    {"2c", 0}, {"2d", 1}, {"2h", 2}, {"2s", 3},
    {"3c", 4}, {"3d", 5}, {"3h", 6}, {"3s", 7},
    {"4c", 8}, {"4d", 9}, {"4h", 10}, {"4s", 11},
    {"5c", 12}, {"5d", 13}, {"5h", 14}, {"5s", 15},
    {"6c", 16}, {"6d", 17}, {"6h", 18}, {"6s", 19},
    {"7c", 20}, {"7d", 21}, {"7h", 22}, {"7s", 23},
    {"8c", 24}, {"8d", 25}, {"8h", 26}, {"8s", 27},
    {"9c", 28}, {"9d", 29}, {"9h", 30}, {"9s", 31},
    {"Tc", 32}, {"Td", 33}, {"Th", 34}, {"Ts", 35},
    {"Jc", 36}, {"Jd", 37}, {"Jh", 38}, {"Js", 39},
    {"Qc", 40}, {"Qd", 41}, {"Qh", 42}, {"Qs", 43},
    {"Kc", 44}, {"Kd", 45}, {"Kh", 46}, {"Ks", 47},
    {"Ac", 48}, {"Ad", 49}, {"Ah", 50}, {"As", 51}  
};
const map<int, str> CARD_NAME = {
    {0, "2c"}, {1, "2d"}, {2, "2h"}, {3, "2s"},
    {4, "3c"}, {5, "3d"}, {6, "3h"}, {7, "3s"},
    {8, "4c"}, {9, "4d"}, {10, "4h"}, {11, "4s"},
    {12, "5c"}, {13, "5d"}, {14, "5h"}, {15, "5s"},
    {16, "6c"}, {17, "6d"}, {18, "6h"}, {19, "6s"},
    {20, "7c"}, {21, "7d"}, {22, "7h"}, {23, "7s"},
    {24, "8c"}, {25, "8d"}, {26, "8h"}, {27, "8s"},
    {28, "9c"}, {29, "9d"}, {30, "9h"}, {31, "9s"},
    {32, "Tc"}, {33, "Td"}, {34, "Th"}, {35, "Ts"},
    {36, "Jc"}, {37, "Jd"}, {38, "Jh"}, {39, "Js"},
    {40, "Qc"}, {41, "Qd"}, {42, "Qh"}, {43, "Qs"},
    {44, "Kc"}, {45, "Kd"}, {46, "Kh"}, {47, "Ks"},
    {48, "Ac"}, {49, "Ad"}, {50, "Ah"}, {51, "As"}
};

inline int get_percent(float value) {
    return roundf(value * 100); 
}
inline int get_rank(int card) {
    return card / NUM_STREETS + 2;
}
inline int get_suit(int card) {
    return card % NUM_STREETS;
}
inline int get_card(int rank, int suit) {
    return NUM_STREETS * rank + suit - 8;
}
inline int get_non_leaf_bucket(int player, int street) {
    return NUM_STREETS * player + street;
}
inline int get_info_id(int player, int street, int bet_id) {
    return bet_id - CUM_BUCKET_SIZE[get_non_leaf_bucket(player, street)];
}
inline pair<int, int> get_thread_range(int total, int num_threads, int thread_id) {
    int num_per_thread = total / num_threads;
    int first_game = thread_id * num_per_thread;
    int last_game = (thread_id == num_threads - 1) ? total - 1 : (thread_id + 1) * num_per_thread - 1;
    return {first_game, last_game};
}
inline lint get_id(arr<int, NUM_FINAL_CARDS> cards, int street) {
    lint id = 0, base = 1;
    for (int i = CUM_STREET_SIZE[street] - 1; i >= 0; i--) {
        id += base * cards[i];
        base *= 100;
    }
    return id;
}
inline arr<int, NUM_FINAL_CARDS> get_cards(lint id, int street) {
    arr<int, NUM_FINAL_CARDS> cards;
    for (int i = CUM_STREET_SIZE[street] - 1; i >= 0; i--) {
        cards[i] = id % 100;
        id /= 100;
    }
    return cards;
}
inline void erase_value(vec<int>& list, int value) {
    list.erase(find(list.begin(), list.end(), value));
}
template<typename T>
inline void append_list(vec<T>& list, const vec<T>& to_append) {
    list.insert(list.end(), to_append.begin(), to_append.end());
}
inline void partial_shuffle(vec<int>& list, int num_to_shuffle) {
    int last_index = list.size() - 1;    
    for (int i = 0; i < num_to_shuffle; i++) {
        int j = get_generator().get_random_int(i, last_index);
        swap(list[i], list[j]);
    }
}
template<typename T>
inline void read(ifstream& file, T& value) {
    file.read(reinterpret_cast<char*>(&value), sizeof(value));
}
template<typename T>
inline void write(ofstream& file, T& value) {
    file.write(reinterpret_cast<char*>(&value), sizeof(value));
}
template<typename T>
inline void read_range(ifstream& file, vec<T>& list) {
    char* ptr = reinterpret_cast<char*>(list.data());
    lint size = list.size() * sizeof(T), read = 0;
    while (read < size) {
        lint to_read = min(GIGABYTE, size - read);
        file.read(ptr + read, to_read);
        if (!file) break;
        read += to_read;
    }
}
template<typename T>
inline void write_range(ofstream& file, vec<T>& list) {
    char* ptr = reinterpret_cast<char*>(list.data());
    lint size = list.size() * sizeof(T), written = 0;
    while (written < size) {
        lint to_write = min(GIGABYTE, size - written);
        file.write(ptr + written, to_write);
        if (!file) break;
        written += to_write;
    }
}

struct Betstate {
    int player = 0, street = 0;
    struct Child {
        Action action = {};
        int bet_id = 0;
    };
    vec<Child> children;
    struct Payoff {
        PayoffType type = {};
        int amount = 0;
    };
    Payoff payoff = {};
};

struct Infoset {
    int num_actions = 0;
    arr<doub, MAX_NUM_ACTIONS> cum_regret = {}, cum_strat = {};
    arr<float, MAX_NUM_ACTIONS> strat = {};
    void upd_strat() {
        doub sum = 0;
        arr<doub, MAX_NUM_ACTIONS> pos_regret;
        for (int i = 0; i < num_actions; i++) {
            pos_regret[i] = max(cum_regret[i], 0.0);
            sum += pos_regret[i];
        }
        if (sum == 0) {
            fill(strat.begin(), strat.begin() + num_actions, 1.0 / num_actions);
            return;
        } 
        for (int i = 0; i < num_actions; i++) 
            strat[i] = pos_regret[i] / sum;
    }
    int get_sampled_action() const {
        float random = get_generator().get_random_float();
        for (int i = 0; i < num_actions; i++) { 
            random -= strat[i]; 
            if (random <= 0) return i; 
        } 
        return num_actions - 1;
    } 
};

struct Blueprint {
    int num_actions = 0;
    arr<float, MAX_NUM_ACTIONS> strat = {};
    void init_from_infoset(const Infoset& info) {
        num_actions = info.num_actions;
        doub sum = 0;
        for (int i = 0; i < num_actions; i++) 
            sum += info.cum_strat[i];
        for (int i = 0; i < num_actions; i++) 
            strat[i] = (sum == 0) ? 1.0 / num_actions : info.cum_strat[i] / sum;
    }
    void init_from_blueprint(const Blueprint& blue, const Betstate& bet, Bias bias, float multiplier) {
        num_actions = blue.num_actions;
        float sum = 0;
        for (int i = 0; i < num_actions; i++) {
            Action action = bet.children[i].action;
            bool do_bias = (bias == Bias::Folding && action == Action::Fold) || 
                           (bias == Bias::CheckCalling && action == Action::CheckCall) ||
                           (bias == Bias::Betting && action != Action::Fold && action != Action::CheckCall);
            strat[i] = blue.strat[i] * (do_bias ? multiplier : 1.0);
            sum += strat[i];
        }
        for (int i = 0; i < num_actions; i++)
            strat[i] /= sum;
    }
    int get_sampled_action() const {
        float random = get_generator().get_random_float();
        for (int i = 0; i < num_actions; i++) { 
            random -= strat[i]; 
            if (random <= 0) return i; 
        } 
        return num_actions - 1;
    } 
};

template<typename T>
struct Map {
    vec<lint> keys;
    vec<T> values;
    T at(lint key) const {
        auto it = lower_bound(keys.begin(), keys.end(), key);
        assert(it != keys.end() && *it == key);
        return values[it - keys.begin()];
    }
};

struct Gamestate {
    Winner winner = {};
    arr<arr<int, NUM_STREETS>, 2> cluster = {};
};



