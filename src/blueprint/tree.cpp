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

struct Child {
    Action action = {};
    int bucket = 0;
    int id_in_bucket = 0;
};

arr<int, NUM_BUCKETS> num_nodes;
arr<vec<int>, NUM_BUCKETS> player, street;
arr<vec<vec<Child>>, NUM_BUCKETS> children;
arr<vec<Betstate::Payoff>, NUM_BUCKETS> payoffs;
pair<int, int> traverse(Playstate play = {}) {
    int bucket = play.is_leaf ? NUM_BUCKETS - 1 : NUM_STREETS * play.player + play.get_street();
    num_nodes[bucket]++;
    int id_in_bucket = num_nodes[bucket] - 1;
    player[bucket].push_back(play.player), street[bucket].push_back(play.get_street());
    children[bucket].push_back({}), payoffs[bucket].push_back({});
    
    if (play.is_leaf) {
        int amount = STACK - play.stack[play.get_opp()];
        // Leaf nodes don't advance round
        bool is_fold = (play.actions[play.round] == Action::Fold);
        Betstate::Payoff payoff = {};
        payoff.type = is_fold ? PayoffType::Fixed : PayoffType:: Unfixed;
        // Leaf nodes don't advance player
        payoff.amount = is_fold ? ((play.player == 0) ? -amount : +amount) : amount;
        payoffs[bucket][id_in_bucket] = payoff;
        return {bucket, id_in_bucket};
    }

    vec<Action> actions = {Action::Fold, Action::CheckCall, Action::AllIn};
    if (play.num_consec_bets < MAX_NUM_CONSEC_BETS) {
        vec<Action> allowed_bets = ALLOWED_BETS[play.get_street()][play.num_consec_bets];
        append_list(actions, allowed_bets);
    }
    
    set<int> amounts_seen;
    vec<Action> allowed_actions;
    for (Action action : actions) {
        int amount_to_action = play.get_action_amount(action);
        if (amount_to_action > play.stack[play.player]) continue;

        bool is_bet = (action != Action::Fold && action != Action::CheckCall);
        bool is_all_in = (action == Action::AllIn);
        if (is_bet && !is_all_in && amount_to_action < play.min_bet) continue;

        // No 2 actions should require the same contribution, apart from folding and checking/calling
        bool is_fold = (action == Action::Fold);
        bool is_check_call = (action == Action::CheckCall); 
        if (!is_fold && !is_check_call && amounts_seen.count(amount_to_action)) continue;
        amounts_seen.insert(amount_to_action);

        allowed_actions.push_back(action);
    }    

    // Children should be ordered and traversed in increasing bet size order
    sort(allowed_actions.begin(), allowed_actions.end());
    vec<Child> child_list;
    for (Action action : allowed_actions) {
        Playstate next = play.get_next_state(action);
        pair<int, int> child_info = traverse(next);
        child_list.push_back({action, child_info.first, child_info.second});
    }
    payoffs[bucket][id_in_bucket] = {PayoffType::NonLeaf, 0}, children[bucket][id_in_bucket] = child_list;
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
