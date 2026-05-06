/*
Generates data to visualise the solution at any given betstate. The amount each cell is filled is the probability
that a player who was dealt those cards reached that betstate, but normalised so that the most probable cell is 
fully filled.
*/

#include <cctype>
#include <iostream>
#include <numeric>
#include "../../include/solver.h"
#include "../../include/init.h"
#include "../../include/convert.h"
#include "../../include/search.h"
#include "../../include/rank.h"
using namespace std;

const str VISUALISE_FILE = "../data/visualise.txt";
const map<str, bool> VISUALISE_VALUE = {
    {"No", false}, {"Yes", true}  
};
const map<bool, str> VISUALISE_NAME = {
    {false, "No"}, {true, "Yes"}  
};

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
void print_visualise_options() {
    cout << "> Visualise options: ";
    for (bool x : {false, true})
        cout << VISUALISE_NAME.at(x) << " ";
    cout << endl;
}
void print_cards(const arr<int, NUM_FINAL_CARDS>& cards, int street) {
    str capitalised_name = STREET_NAME.at((Street) street);
    capitalised_name[0] = toupper(capitalised_name[0]);
    cout << "> " << capitalised_name << " cards: ";
    for (int i = NUM_HOLE_CARDS; i < CUM_STREET_SIZE[street]; i++) {
        int card = cards[i];
        cout << CARD_NAME.at(card) << " ";
    }
    cout << endl;
}
void print_actions(const Betstate& bet, const Playstate& play) {
    cout << "> Actions: ";
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
void print_game_info(const Playstate& play) {
    cout << "> ";
    for (int i : {0, 1}) 
        cout << POSITION_NAME.at(i) << " stack: $" << play.stack[i] << " | ";
    cout << "Pot: $" << play.pot << endl;
}
void print_visualise(const arr<bool, NUM_CARDS>& in_public, arr<int, NUM_FINAL_CARDS> cards, int bet_id) {
    const Betstate& bet = betstate[bet_id];
    int player = bet.player, street = bet.street, info_id = get_info_id(player, street, bet_id);
    bool at_preflop = (street == (int) Street::Preflop);
    int num_actions = bet.children.size();
    int num_clusters = NUM_CLUSTERS[(int) Street::Preflop];    

    ofstream file(VISUALISE_FILE);
    file << num_actions << endl;
    for (Betstate::Child child : bet.children)
        file << ACTION_NAME.at(child.action) << " ";
    file << endl;
    
    struct Policy{
        int count = 0;
        float reach_prob = 0;
        arr<float, MAX_NUM_ACTIONS> strat = {};
    };
    vec<Policy> policy(num_clusters);

    auto get_cluster = [](arr<int, NUM_FINAL_CARDS> cards, int street) {
        cards = convert(cards, street);
        lint id = get_id(cards, street);
        return clusters[street].at(id);
    };
    for (int i = 0; i < NUM_CARDS; i++) {
        for (int j = i + 1; j < NUM_CARDS; j++) {
            if (in_public[i] || in_public[j]) continue;
            cards[0] = i, cards[1] = j;

            int current_cluster = get_cluster(cards, street);
            Blueprint blue = blueprint[(int) Bias::Base][player][street][info_id][current_cluster];
            if (!at_preflop) {
                const Infoset& info = infoset[player][info_id][current_cluster];
                blue.init_from_infoset(info);
            }

            int preflop_cluster = get_cluster(cards, (int) Street::Preflop);
            policy[preflop_cluster].count++;
            policy[preflop_cluster].reach_prob += get_reach_prob({i, j}, player);
            for (int k = 0; k < num_actions; k++)
                policy[preflop_cluster].strat[k] += blue.strat[k];
        }
    }

    float max_reach_prob = 0;
    for (int i = 0; i < num_clusters; i++) {
        policy[i].reach_prob /= max(policy[i].count, 1);
        max_reach_prob = max(max_reach_prob, policy[i].reach_prob);
    }

    for (int i = 0; i < num_clusters; i++) {
        float relative_prob = policy[i].reach_prob / max_reach_prob;
        file << relative_prob << " ";
        for (int j = 0; j < num_actions; j++) {
            policy[i].strat[j] /= max(policy[i].count, 1);
            file << policy[i].strat[j] << " ";
        }
        file << endl;
    }
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
    for (int street = 0; street < NUM_STREETS; street++) {
        bool at_preflop = (street == (int) Street::Preflop);
        if (!at_preflop) {
            vec<str> inputs = get_input(CARD_VALUE, "Input " + STREET_NAME.at((Street) street) + " cards:", STREET_SIZE[street]);
            for (int i = 0; i < STREET_SIZE[street]; i++) {
                int index = CUM_STREET_SIZE[street - 1] + i;
                cards[index] = CARD_VALUE.at(inputs[i]);
                in_public[cards[index]] = true;
                erase_value(remaining_cards, cards[index]);
            }
            print_cards(cards, street);
            search(cards, in_public, remaining_cards, bet_id);
        }
       
        Betstate bet = betstate[bet_id];
        while (bet.street == street && bet.payoff.type == PayoffType::NonLeaf) {
            print_visualise_options();
            str visualise_input = get_input(VISUALISE_VALUE, "Choose visualise option:", 1).back();
            bool is_visualise = VISUALISE_VALUE.at(visualise_input);
            if (is_visualise) {
                print_visualise(in_public, cards, bet_id);
                return;
            }

            print_game_info(play);
            print_actions(bet, play);
            str action_input = get_input(ACTION_VALUE, "Choose action:", 1).back();
            int action_id = get_action_id(bet, ACTION_VALUE.at(action_input));

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