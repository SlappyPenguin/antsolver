/*
Builds the tree of possible bet sequences (not accounting for cards), according to our chosen bet abstraction:

1) At any time, folding and checking/calling are allowed.

2) Unless the last move was all-in, going all-in is allowed.

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

4) Apart from folding and checking/calling, we only allow the first action which requires a certain amount of 
contribution to pot.

NOTE: bet/raise sizes are expressed as % of pot. However, for raises, this pot size is the hypothetical size after
we've called the opponent's bet (https://blog.gtowizard.com/how-to-calculate-raises-in-poker/).
*/

#include <bits/stdc++.h>
#include "../../include/solver.h"
using namespace std;

constexpr int MAX_NUM_CONSEC_BETS = 3;
constexpr int NUM_ROUNDS_PER_STREET = 6;
const str BETSTATES_FILE = "../data/betstates.bin";
const arr<arr<vec<Action>, MAX_NUM_CONSEC_BETS>, NUM_STREETS> ALLOWED_BETS = {{
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
    arr<Action, NUM_STREETS * NUM_ROUNDS_PER_STREET> actions = {};
    int round = 0, player = 0;
    bool is_leaf = false;
    int pot = SMALL_BLIND + BIG_BLIND;
    arr<int, 2> stack = {STACK - SMALL_BLIND, STACK - BIG_BLIND};
    arr<int, 2> contribution = {SMALL_BLIND, BIG_BLIND};
    int num_consec_bets = 0, min_bet = BIG_BLIND;

    inline int get_opp() const {
        return !player;
    }
    inline int get_street() const {
        return round / NUM_ROUNDS_PER_STREET;
    }
    inline bool is_street_start() const {
        return (round % NUM_ROUNDS_PER_STREET == 0);
    }
    inline int get_check_call_amount(Action action) const {
        return contribution[!player] - contribution[player];
    }
    inline int get_action_amount(Action action) const {
        int amount_to_check_call = get_check_call_amount(action);
        int amount_to_action = 0;
        int pot_after_check_call = pot + amount_to_check_call;
        switch (action) {
            case Action::Fold: amount_to_action = 0; break;
            case Action::CheckCall: amount_to_action = amount_to_check_call; break;
            case Action::AllIn: amount_to_action = stack[player]; break;
            default: amount_to_action = amount_to_check_call + 
                                        pot_after_check_call * POT_PERCENT[(int) action - (int) Action::Fold];
        }
        return amount_to_action;
    }
    // Does not check for whether we actually are allowed to do this action
    State get_next_state(Action action) const {
        State next = *this;
        next.actions[round] = action;

        int amount_to_check_call = get_check_call_amount(action);
        int amount_to_action = get_action_amount(action);
        next.stack[player] -= amount_to_action;
        next.contribution[player] += amount_to_action, next.pot += amount_to_action;

        bool is_bet = (action != Action::Fold && action != Action::CheckCall);
        next.num_consec_bets = (is_bet) ? num_consec_bets + 1 : 0;
        next.min_bet = max(amount_to_action - amount_to_check_call, BIG_BLIND);

        bool is_fold = (action == Action::Fold);
        bool is_last_call_in_street = (!is_street_start() && action == Action::CheckCall);
        bool is_all_in_call = (is_last_call_in_street && actions[round - 1] == Action::AllIn);
        bool is_last_call_in_game = (is_last_call_in_street && get_street() == (int) Street::River);
        
        if (is_fold || is_all_in_call || is_last_call_in_game) {
            next.is_leaf = true;
        } else if (is_last_call_in_street) {
            next.round = round - (round % NUM_ROUNDS_PER_STREET) + NUM_ROUNDS_PER_STREET;
            next.player = 1;
            next.contribution = {};
            next.num_consec_bets = 0;
            next.min_bet = BIG_BLIND;
        } else {
            next.round++;
            next.player = get_opp();
        }
        return next;
    }
    void print() {
        cout << "New state ==================================" << endl;
        cout << "Actions: ";
        for (int i = 0; i < NUM_STREETS * NUM_ROUNDS_PER_STREET; i++)
            cout << ACTION_NAME.at(actions[i]) << " ";
        cout << endl;
        cout << "Round: " << round << endl;
        cout << "Player: " << player << endl;
        cout << "Pot: " << pot << endl;
        cout << "Contributions: " << contribution[0] << " " << contribution[1] << endl;
    }
};
struct Child {
    Action action = {};
    int bucket = 0;
    int id_in_bucket = 0;
};

arr<int, NUM_BUCKETS> num_nodes;
arr<vec<int>, NUM_BUCKETS> player, street;
arr<vec<vec<Child>>, NUM_BUCKETS> children;
arr<vec<Betstate::Payoff>, NUM_BUCKETS> payoffs;
pair<int, int> traverse(State state = {}) {
    int bucket = state.is_leaf ? NUM_BUCKETS - 1 : NUM_STREETS * state.player + state.get_street();
    num_nodes[bucket]++;
    int id_in_bucket = num_nodes[bucket] - 1;
    player[bucket].push_back(state.player), street[bucket].push_back(state.get_street());
    
    if (state.is_leaf) {
        int amount = STACK - state.stack[state.get_opp()];
        // Leaf nodes don't advance round
        bool is_fold = (state.actions[state.round] == Action::Fold);
        Betstate::Payoff payoff = {};
        payoff.type = is_fold ? PayoffType::Fixed : PayoffType:: Unfixed;
        // Leaf nodes don't advance player
        payoff.amount = is_fold ? ((state.player == 0) ? -amount : +amount) : amount;
        payoffs[bucket].push_back(payoff), children[bucket].push_back({});
        return {bucket, id_in_bucket};
    }

    vec<Action> actions = {Action::Fold, Action::CheckCall, Action::AllIn};
    if (state.num_consec_bets < MAX_NUM_CONSEC_BETS) {
        vec<Action> allowed_bets = ALLOWED_BETS[state.get_street()][state.num_consec_bets];
        append_list(actions, allowed_bets);
    }
    
    set<int> amounts_seen;
    vec<Child> child_list;
    for (Action action : actions) {
        int amount_to_action = state.get_action_amount(action);

        if (amount_to_action > state.stack[state.player]) continue;

        // No 2 actions should require the same contribution, apart from folding and checking/calling
        bool is_fold = (action == Action::Fold);
        bool is_check_call = (action == Action::CheckCall); 
        if (!is_fold && !is_check_call && amounts_seen.count(amount_to_action)) continue;
        amounts_seen.insert(amount_to_action);

        bool is_bet = (action != Action::Fold && action != Action::CheckCall);
        bool is_all_in = (action == Action::AllIn);
        if (is_bet && !is_all_in && amount_to_action < state.min_bet) continue;

        State next = state.get_next_state(action);
        pair<int, int> child_info = traverse(next);
        child_list.push_back({action, child_info.first, child_info.second});
    }    
    payoffs[bucket].push_back({PayoffType::NonLeaf, 0}), children[bucket].push_back(child_list);
    return {bucket, id_in_bucket};
}

void print_betstates() {
    arr<int, NUM_BUCKETS> cum_bucket_size = {};
    for (int i = 1; i < NUM_BUCKETS; i++)
        cum_bucket_size[i] = cum_bucket_size[i - 1] + num_nodes[i - 1];
    
    ofstream file(BETSTATES_FILE, ios::binary);
    for (int i = 0; i < NUM_BUCKETS; i++) {
        for (int j = 0; j < num_nodes[i]; j++) {
            write(file, player[i][j]), write(file, street[i][j]);
            int num_children = children[i][j].size();
            write(file, num_children);
            for (int k = 0; k < num_children; k++) {
                const Child& child = children[i][j][k];
                Action action = child.action;
                int id = cum_bucket_size[child.bucket] + child.id_in_bucket;
                write(file, action), write(file, id);
            }
            write(file, payoffs[i][j].type), write(file, payoffs[i][j].amount);
        }
    }
}

int main() {
    cout << "Generating for... ";
    auto start_time = chrono::high_resolution_clock::now();

    traverse();
    print_betstates();

    auto end_time = chrono::high_resolution_clock::now();
    chrono::duration<doub> elapsed_time = end_time - start_time;
    cout << fixed << setprecision(3);
    cout << elapsed_time.count() << " seconds" << '\n';
}
