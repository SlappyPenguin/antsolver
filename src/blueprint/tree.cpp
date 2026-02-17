/*
Builds the tree of possible bet sequences (not accounting for cards), according to our chosen bet abstraction:

1) At any time, folding, checking/calling and going all-in are allowed.

2) Bet/raise sizes are expressed as % of pot. However, for raises, this pot size is the hypothetical size after
we've called the opponent's bet (https://blog.gtowizard.com/how-to-calculate-raises-in-poker/).

3) Additionally, for the first 3 bets of each street, these are the allowed sizes:
    Preflop:
        1st: 50%, 100%
        2nd: 50%, 100%
        3rd: 50%, 100%
    Flop:
        1st: 25%, 50%, 75%, 100%
        2nd: 50%, 100%
        3rd: 50%, 100%
    Turn:
        1st: 50%, 75%, 100%, 200%, 400%
        2nd: 50%, 100%
        3rd: 100%
    River:
        1st: 50%, 75%, 100%, 200%, 400%
        2nd: 50%, 100%, 200%, 400%
        3rd: 100%
*/

#include <bits/stdc++.h>
#include "../../include/solver.h"
using namespace std;

constexpr arr<arr<vec<Action>, 3>, 4> BETS = {{
    {vec<Action>{Action::Bet50, Action::Bet100}, 
     vec<Action>{Action::Bet50, Action::Bet100}, 
     vec<Action>{Action::Bet50, Action::Bet100}},

    {vec<Action>{Action::Bet25, Action::Bet50, Action::Bet75, Action::Bet100}, 
     vec<Action>{Action::Bet50, Action::Bet100}, 
     vec<Action>{Action::Bet50, Action::Bet100}},

    {vec<Action>{Action::Bet50, Action::Bet75, Action::Bet100, Action::Bet200, Action::Bet400}, 
     vec<Action>{Action::Bet50, Action::Bet100}, 
     vec<Action>{Action::Bet100}},

    {vec<Action>{Action::Bet50, Action::Bet75, Action::Bet100, Action::Bet200, Action::Bet400}, 
     vec<Action>{Action::Bet50, Action::Bet100, Action::Bet200, Action::Bet400}, 
     vec<Action>{Action::Bet100}}
}};


struct State {
    arr<Action, 24> actions = {NOTHING};
    int round = 0, player = 0;
    int pot = SMALL_BLIND + BIG_BLIND;
    arr<int, 2> bet = {SMALL_BLIND, BIG_BLIND};

    inline int get_opp() {
        return !player;
    }
    inline int get_stack() {
        return STACK - bet[player] - (pot - bet[player] - bet[get_opp()]) / 2;
    }
    inline int get_street() {
        return round / 6;
    }
    bool is_leaf() {
        // If the game ends prematurely (fold or all-in call) round always moves up by 1
        if (round == 0) return false;
        Action last_action = actions[round - 1];
        if (last_action == FOLD) return true;
        
        if (round == 1) return false;
        Action last_action2 = actions[round - 2];
        if (last_action2 == ALL_IN && last_action == CHECK_CALL) return true;

        if (round == 24) return true;
        return false;
    }
    State get_next(Action action, int req) {
        State new_state = {actions, round, player, pot, bet};
        
        new_state.actions[round] = action;
        new_state.pot += req, new_state.bet[player] += req; 
        
        // If the game ends prematurely (fold or all-in call) round always moves up by 1
        bool new_street = (action == CHECK_CALL && round % 6 != 0 && actions[round - 1] != ALL_IN);
        if (!new_street) {
            new_state.round++; 
            new_state.player = get_opp();
        } else {
            new_state.round = round - (round % 6) + 6;
            new_state.player = 1;
            new_state.bet = {0, 0};
        }
        return new_state;
    }
    void print() {
        cout << "New state-------------------------" << endl;
        cout << "Actions: ";
        for (int i = 0; i <= 23; i++)
            cout << actions[i] << " ";
        cout << endl;
        cout << "Round: " << round << endl;
        cout << "Player: " << player << endl;
        cout << "Pot: " << pot << endl;
        cout << "Bets: " << bet[0] << " " << bet[1] << endl;
    }
};

arr<int, 9> num_nodes;
arr<int, NUM_NODES> player, street;
arr<vec<pair<Action, int>>, NUM_NODES> children;
arr<pair<Type, int>, NUM_NODES> payoff;
int traverse(State state = {}, int min_bet = BIG_BLIND, int num_bets = 0) {
    int bucket = (state.is_leaf()) ? 8 : 4 * state.player + state.get_street(); 
    num_nodes[bucket]++;
    int id = num_nodes[bucket] - 1 + CUM_BUCKET_SIZE[bucket];
    player[id] = state.player, street[id] = state.get_street();
    
    if (state.is_leaf()) {
        int opp_stack = 2 * STACK - state.pot - state.get_stack();
        int amount = STACK - opp_stack;
        if (state.actions[state.round - 1] == FOLD) {
            payoff[id] = {FIXED, (state.player == 0) ? amount : -amount};
        } else payoff[id] = {UNFIXED, amount}; // If it ends with CHECK_CALL, both stand to win the same
        return id;
    }
    payoff[id] = {NON_LEAF, 0};

    vec<Action> actions;
    actions.push_back(FOLD), actions.push_back(CHECK_CALL);
    bool can_bet = (state.round == 0 || state.actions[state.round - 1] != ALL_IN);
    if (can_bet) {
        if (num_bets <= 2) 
            actions.insert(actions.end(), BETS[state.get_street()][num_bets].begin(), BETS[state.get_street()][num_bets].end());
        actions.push_back(ALL_IN);
    }
    
    int player = state.player, opp = state.get_opp();
    for (Action action : actions) {
        int stack = state.get_stack();
        int req = 0, req_check_call = state.bet[opp] - state.bet[player]; 
        int new_pot = state.pot + req_check_call;

        switch (action) {
            case FOLD: req = 0; break;
            case CHECK_CALL: req = req_check_call; break;
            case ALL_IN: req = stack; break;
            default: req = req_check_call + roundf(new_pot * MULTIPLIER[action - BET_25]); break;
        }

        if (req > stack) continue;
        // No 2 actions should have the same req except for FOLD and CHECK_CALL
        if (req == stack && action != ALL_IN && action != CHECK_CALL) continue;

        bool is_bet = (action != FOLD && action != CHECK_CALL);
        if (is_bet && action != ALL_IN && req - req_check_call < min_bet) continue;

        State new_state = state.get_next(action, req);
        int new_min_bet = (new_state.round % 6 == 0) ? BIG_BLIND : max(req - req_check_call, BIG_BLIND);
        int new_num_bets = is_bet ? num_bets + 1 : 0;
        int child_id = traverse(new_state, new_min_bet, new_num_bets);
        children[id].push_back({action, child_id});
    }    
    return id;
}

int main() {
    traverse();

    ofstream out("betstates.bin", ios::binary);
    auto write = [&out](auto& x) {
        out.write(reinterpret_cast<char*>(&x), sizeof(x));
    };
    for (int i = 0; i <= NUM_NODES - 1; i++) {
        write(player[i]), write(street[i]);
        int num_children = children[i].size(); write(num_children);
        for (int j = 0; j <= num_children - 1; j++) 
            write(children[i][j].fir), write(children[i][j].sec);
        write(payoff[i].fir), write(payoff[i].sec);
    }
}
