/*
Implements external sampling MCCFR from https://mlanctot.info/files/papers/PhD_Thesis_MarcLanctot.pdf in order to 
find the Nash equilibrium for Kuhn Poker. External sampling MCCFR samples both chance and opponent actions, which
significantly reduces the time for each traversal.

Solution verified from https://webdocs.cs.ualberta.ca/~holte/Publications/aaai2005poker.pdf.
*/

#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <map>
#include <random>
#include <string>
#include <utility>
#include <vector>
using namespace std;
#define str string 
#define doub double 
#define vec vector
#define fir first 
#define sec second

constexpr int NUM_ITERATIONS = 8e5, INF = 1e9;

struct Infoset {
    map<char, doub> cum_regret, cum_strat;
    map<char, doub> strat = {{'p', 0.5}, {'b', 0.5}};
};
map<str, Infoset> infoset;

bool is_leaf(str state) {
    if (state.size() <= 1) return false;
    char x = state[state.size() - 2], y = state[state.size() - 1];
    return (x == 'b' || (x == 'p' && y == 'p'));
}
int get_utility(int learner, str state) {
    char w = state[0], x = state[1], y = state[state.size() - 2], z = state[state.size() - 1];
    int payoff_for_0 = 0;

    if (y == 'b' && z == 'p') {
        if (state.size() == 4) payoff_for_0 = 1;
        else payoff_for_0 = -1;
    } else {
        int winner = (w == 'a' || x == 'q') ? 0 : 1;
        int payoff = (z == 'b') ? 2 : 1;
        payoff_for_0 = (winner == 0) ? payoff : -payoff;
    }
    return ((learner == 0) ? payoff_for_0 : -payoff_for_0);   
}

bool is_chance(str state) { 
    return (state.size() <= 1);
}
int get_random(int size) {
    static random_device device;
    static mt19937 generator(device()); 
    uniform_int_distribution<int> distribution(0, size - 1);
    return distribution(generator);
}
str get_chance_state(str state) {
    vec<char> option = {'a', 'k', 'q'};
    if (state.size() == 1) option.erase(find(option.begin(), option.end(), state[0]));
    return state + option[get_random(option.size())]; 
}

str get_infoset(int player, str state) {
    if (player == 0) state[1] = '-';
    else state[0] = '-';
    return state;
}
void upd_strat(str info) {
    doub regret_sum = 0;
    for (char action : {'p', 'b'}) {
        doub pos_regret = max(infoset[info].cum_regret[action], 0.0);
        infoset[info].strat[action] = pos_regret;
        regret_sum += pos_regret;
    }
    for (char action : {'p', 'b'}) {
        if (regret_sum == 0) infoset[info].strat[action] = 0.5;
        else infoset[info].strat[action] /= regret_sum;
    }
}

char get_action(str info) {
    static random_device device; 
    static mt19937 generator(device());
    static uniform_real_distribution<doub> distribution(0, 1);
    doub random = distribution(generator);
    for (char action : {'p', 'b'}) {
        random -= infoset[info].strat[action];
        if (random <= 0) return action;
    }
    return 'b';
}

// Due to external sampling, EV here is never accurate
doub get_value(int iteration, int learner, str state) {
    if (is_leaf(state)) 
        return get_utility(learner, state);
    else if (is_chance(state)) 
        return get_value(iteration, learner, get_chance_state(state));

    int player = state.size() % 2;
    str info = get_infoset(player, state);
    upd_strat(info);
    
    doub value = 0;
    if (player == learner) {
        map<char, doub> action_ev;
        for (char action : {'p', 'b'}) {
            action_ev[action] = get_value(iteration, learner, state + action);
            value += action_ev[action] * infoset[info].strat[action];
        }
        for (char action : {'p', 'b'}) 
            infoset[info].cum_regret[action] += action_ev[action] - value;
    } else {
        value = get_value(iteration, learner, state + get_action(info));
        for (char action : {'p', 'b'}) 
            infoset[info].cum_strat[action] += infoset[info].strat[action];
    }
    return value;
}

void final_upd_strat() {
    for (pair<const str, Infoset>& info : infoset) {
        doub strat_sum = 0;
        for (char action : {'p', 'b'}) 
            strat_sum += info.sec.cum_strat[action];
        for (char action : {'p', 'b'}) 
            info.sec.strat[action] = info.sec.cum_strat[action] / strat_sum;
    }
}
void print_strat() {
    cout << "Equilibrium strategy: " << '\n';
    for (pair<const str, Infoset>& info : infoset) 
        cout << info.fir << ": p" << info.sec.strat['p'] << " " << "b" << info.sec.strat['b'] << '\n';
}
vec<pair<str, doub>> get_chances(str state) {
    vec<char> option = {'a', 'k', 'q'};
    if (state.size() == 1) option.erase(find(option.begin(), option.end(), state[0]));
    vec<pair<str, doub>> ans;
    for (char x : option) 
        ans.push_back({state + x, 1 / (doub) option.size()});
    return ans;
}
doub get_ev(int learner, str state) {
    if (is_leaf(state)) 
        return get_utility(learner, state);
    else if (is_chance(state)) {
        doub ev = 0;
        for (pair<str, doub> chance : get_chances(state)) 
            ev += chance.sec * get_ev(learner, chance.fir);
        return ev;
    }

    int player = state.size() % 2;
    str info = get_infoset(player, state);

    doub ev = 0;
    for (char action : {'p', 'b'}) 
        ev += infoset[info].strat[action] * get_ev(learner, state + action);
    return ev;
}
void print_ev0() {
    cout << "Expected value for player 1: ";
    cout << get_ev(0, "") << '\n';
}

int main() {
    cout << fixed << setprecision(3);
    cout << "Training for... ";
    auto start_time = chrono::high_resolution_clock::now();
    
    for (int i = 0; i < NUM_ITERATIONS; i++) 
        for (int j : {0, 1}) 
            get_value(i, j, "");

    auto end_time = chrono::high_resolution_clock::now();
    chrono::duration<doub> elapsed_time = end_time - start_time;
    cout << elapsed_time.count() << " seconds" << '\n';

    final_upd_strat(), print_strat(), print_ev0();
}

