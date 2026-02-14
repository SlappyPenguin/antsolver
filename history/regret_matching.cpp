/*
Implements regret matching from https://www.ma.imperial.ac.uk/~dturaev/neller-lanctot.pdf in order to find the 
Nash equilibrium for Rock Paper Scissors. Regret matching picks actions with probability proportional to their
nonnegative cumulative regret.
*/

#include <bits/stdc++.h>
using namespace std;
#define arr array 
#define doub double 

constexpr int NUM_ACTIONS = 3, NUM_ITERATIONS = 1e7;

arr<arr<doub, NUM_ACTIONS>, 2> cum_regret, cum_strat, strat;

void upd_strat(int player) {
    doub regret_sum = 0;
    for (int i = 0; i < NUM_ACTIONS; i++) {
        doub pos_regret = max(cum_regret[player][i], 0.0);
        strat[player][i] = pos_regret;
        regret_sum += pos_regret;
    }
    for (int i = 0; i < NUM_ACTIONS; i++) {
        if (regret_sum == 0) strat[player][i] = 1 / (doub) NUM_ACTIONS;
        else strat[player][i] /= regret_sum;
        cum_strat[player][i] += strat[player][i];
    }
}
int get_action(int player) {
    static random_device device;
    static mt19937 generator(device());
    static uniform_real_distribution<doub> distribution(0, 1); 
    doub random = distribution(generator);
    for (int i = 0; i < NUM_ACTIONS; i++) {
        random -= strat[player][i];
        if (random <= 0) return i;
    }
    return NUM_ACTIONS - 1;
}

// Rock = 0, Paper = 1, Scissors = 2
int get_utility(int my_action, int your_action) {
    if (my_action == your_action) return 0;
    if ((my_action - your_action + NUM_ACTIONS) % NUM_ACTIONS == 1) return 1;
    else return -1;
}

void train(int iteration) {
    arr<int, 2> action = {};
    for (int i : {0, 1}) {
        upd_strat(i);
        action[i] = get_action(i);
    }

    arr<arr<doub, NUM_ACTIONS>, 2> utility = {};
    for (int i : {0, 1}) {
        int j = (i == 0) ? 1 : 0;
        for (int k = 0; k < NUM_ACTIONS; k++) 
            utility[i][k] = get_utility(k, action[j]);
    }

    for (int i : {0, 1}) {
        for (int j = 0; j < NUM_ACTIONS; j++) {
            doub regret = utility[i][j] - utility[i][action[i]];
            cum_regret[i][j] += regret;
        }
    }
}

void final_upd_strat() {
    for (int player : {0, 1}) {
        doub strat_sum = 0;
        for (int i = 0; i < NUM_ACTIONS; i++) 
            strat_sum += cum_strat[player][i];
        for (int i = 0; i < NUM_ACTIONS; i++) 
            strat[player][i] = cum_strat[player][i] / strat_sum;
    }
}
void print_strat() {
    for (int player : {0, 1}) {
        cout << "Equilibrium strategy for player " << player + 1 << ": ";
        for (int i = 0; i < NUM_ACTIONS; i++) 
            cout << strat[player][i] << " ";
        cout << '\n';
    }   
}

int main() {
    cout << fixed << setprecision(3);
    cout << "Training for... ";
    auto start_time = chrono::high_resolution_clock::now();

    for (int i = 0; i < NUM_ITERATIONS; i++) 
        train(i); 

    auto end_time = chrono::high_resolution_clock::now();
    chrono::duration<doub> elapsed_time = end_time - start_time;
    cout << elapsed_time.count() << " seconds" << '\n';

    final_upd_strat(), print_strat();
}