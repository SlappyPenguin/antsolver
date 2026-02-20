/*
Implements depth limited solving from https://arxiv.org/pdf/1805.08195, with the following modifications:

- Solving is not done on the preflop.
- We only resolve the abstracted subgame, i.e. off-tree actions are not allowed.
- MCCFR is always used for resolving.
- Leaf node values are evaluated using the bias strategy and random rollouts.
- Unsafe solving is used, with probabilities based off the most recent search, if possible.
- The player who gets to alter their strategy after leaf nodes is determined randomly, similar to in 
  https://noambrown.github.io/papers/19-Science-Superhuman_Supp.pdf.

NOTE: This code is not thread-safe, i.e. race conditions can happen. From testing it seems this is faster and has 
no noticable effect on correctness.
*/

#include <bits/stdc++.h>
#include "../../include/solver.h"
#include "../../include/init.h"
#include "../../include/convert.h"
#include "../../include/search.h"
#include "../../include/rank.h"
using namespace std;

arr<Betstate, NUM_BETSTATES> betstate;
arr<arr<arr<arr<vec<Blueprint>, MAX_BUCKET_SIZE>, NUM_STREETS>, 2>, NUM_BIASES> blueprint;
arr<arr<vec<Infoset>, MAX_BUCKET_SIZE>, 2> infoset;
arr<arr<vec<Infoset>, MAX_BUCKET_SIZE>, 2> frontier;
arr<Map<short>, NUM_STREETS> clusters;
void init() {
    init_ranks();
    init_betstate(betstate);
    init_blueprint(betstate, blueprint);
    init_clusters(clusters);
}

template<typename T>
vec<str> get_input(const map<str, T>& input_map, str prompt, int num_inputs) {
    cout << "> " << prompt << " ";
    vec<str> ans;
    while (true) {
        ans.clear();
        for (int i = 0; i < num_inputs; i++) {
            str input; cin >> input;
            ans.push_back(input);
        }
        bool is_valid = true;
        for (str input : ans) 
            if (!input_map.count(input)) 
                is_valid = false;
        if (is_valid) break;
        cout << "> Invalid input, try again: ";
    }
    return ans;
}
void print_new_game(int game) {
    cout << "> Playing game #" << game + 1 << "..." << endl;
}
void print_position(int position) {
    cout << "> My position: " << POSITION_NAME.at(position) << endl;
}
void print_thin_divider() {
    cout << "----------------------------------------" << endl;
}
void print_thick_divider() {
    cout << "========================================" << endl;
}
void print_game_over() {
    cout << "> Game over!" << endl;
}
void print_positions() {
    cout << "> Positions: ";
    for (int i : {0, 1})
        cout << POSITION_NAME.at(i) << " ";
    cout << endl;
}
void print_cards(const arr<int, NUM_FINAL_CARDS>& cards, int street) {
    str capitalised_name = STREET_NAME.at((Street) street);
    capitalised_name[0] = toupper(capitalised_name[0]);
    cout << "> " << capitalised_name << " cards: ";
    for (int i = 0; i < CUM_STREET_SIZE[street]; i++) {
        if (i == NUM_HOLE_CARDS) cout << "| ";
        int card = cards[i];
        cout << CARD_NAME.at(card) << " ";
    }
    cout << endl;
}
void print_my_actions(const Betstate& bet, const Playstate& play, const Blueprint& blue) {
    cout << "> My actions: ";
    for (int i = 0; i < (int) bet.children.size(); i++) {
        Action action = bet.children[i].action;
        int amount_to_action = play.get_action_amount(action);
        cout << ACTION_NAME.at(action) << "($" << amount_to_action << ", " << get_percent(blue.strat[i]) <<  "%) ";
    }
    cout << endl;
}
void print_opp_actions(const Betstate& bet, const Playstate& play) {
    cout << "> Opponent actions: ";
    for (int i = 0; i < (int) bet.children.size(); i++) {
        Action action = bet.children[i].action;
        int amount_to_action = play.get_action_amount(action);
        cout << ACTION_NAME.at(action) << "($" << amount_to_action << ") ";
    }
    cout << endl;
}
int get_action_id(const Betstate& bet, Action action) {
    for (int i = 0; i < (int) bet.children.size(); i++)
        if (bet.children[i].action == action)
            return i;
    assert(false); return -1;
}
void print_game_info(const Playstate& play, int position) {
    cout << "> " << "My stack: $" << play.stack[position];
    cout << " | Opponent stack: $" << play.stack[!position];
    cout << " | Pot: $" << play.pot << endl;
}

void play(int game) {   
    init_reach_prob();

    arr<int, NUM_FINAL_CARDS> cards = {};
    vec<int> remaining_cards(NUM_CARDS);
    iota(remaining_cards.begin(), remaining_cards.end(), 0);
    arr<bool, NUM_CARDS> in_public = {};
    int bet_id = 0;
    Playstate play = {};

    print_thick_divider();
    print_new_game(game);
    print_positions();
    int position = POSITION_VALUE.at(get_input(POSITION_VALUE, "Input my position:", 1).back());
    for (int street = 0; street < NUM_STREETS; street++) {
        bool at_preflop = (street == (int) Street::Preflop);
        vec<str> inputs = get_input(CARD_VALUE, "Input " + STREET_NAME.at((Street) street) + " cards:", STREET_SIZE[street]);
        for (int i = 0; i < STREET_SIZE[street]; i++) {
            int index = at_preflop ? i : CUM_STREET_SIZE[street - 1] + i;
            cards[index] = CARD_VALUE.at(inputs[i]);
            
            if (at_preflop) continue;
            in_public[cards[index]] = true;
            erase_value(remaining_cards, cards[index]);
        }
        print_cards(cards, street);

        if (!at_preflop) search(cards, in_public, remaining_cards, bet_id);
       
        Betstate bet = betstate[bet_id];
        lint id = get_id(convert(cards, street), street);
        int cluster = clusters[street].at(id);
        while (bet.street == street && bet.payoff.type == PayoffType::NonLeaf) {
            int player = bet.player, info_id = get_info_id(player, street, bet_id);

            int action_id;
            if (player == position) {
                Blueprint blue;
                if (!at_preflop) {
                    const Infoset& info = infoset[player][info_id][cluster];
                    blue.init_from_infoset(info);

                    for (int i = 0; i < info.num_actions; i++) 
                        cout << info.cum_regret[i] << " " << info.cum_strat[i] << endl;
                } else {
                    blue = blueprint[(int) Bias::Base][player][street][info_id][cluster];
                }

                print_game_info(play, position);
                print_my_actions(bet, play, blue);
                str input = get_input(ACTION_VALUE, "Choose my action:", 1).back();
                action_id = get_action_id(bet, ACTION_VALUE.at(input));
            } else {
                if (!at_preflop) {
                    const Infoset& info = infoset[player][info_id][cluster];
                    for (int i = 0; i < info.num_actions; i++) 
                        cout << info.cum_regret[i] << " " << info.cum_strat[i] << endl;
                }

                print_opp_actions(bet, play);
                str input = get_input(ACTION_VALUE, "Choose opponent action:", 1).back();
                action_id = get_action_id(bet, ACTION_VALUE.at(input));
            }

            update_reach_prob(in_public, cards, bet_id, action_id);
            play = play.get_next_state(bet.children[action_id].action);
            bet_id = bet.children[action_id].bet_id;
            bet = betstate[bet_id];           
        }
        print_thin_divider();
        if (bet.payoff.type != PayoffType::NonLeaf) break;
    }
    print_game_over();
}

int main() {
    init();
    for (int game = 0; ; game++) 
        play(game);
}