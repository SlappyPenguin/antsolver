#include <bits/stdc++.h>
#include "../../include/random.h"
#include "../../include/solver.h"
#include "../../include/retrainer.h"
using namespace std;

static inline float get_leaf_value(const Betstate::Payoff& payoff, const Gamestate& game, int learner) {
    Winner winner = game.winner;
    int payoff_for_0 = 0; 
    if (payoff.type == PayoffType::Fixed || winner == Winner::Zero) payoff_for_0 = payoff.amount;
    else if (winner == Winner::One) payoff_for_0 = -payoff.amount;
    return (learner == 0) ? payoff_for_0 : -payoff_for_0;
}

static inline float estimate_value(
    const Gamestate& game,
    arr<int, 2> biases,
    int bet_id, int learner
) {
    while (true) {
        const Betstate& bet = betstate[bet_id];
        const Betstate::Payoff& payoff = bet.payoff;
        if (payoff.type != PayoffType::NonLeaf) 
            return get_leaf_value(payoff, game, learner);

        int player = bet.player, street = bet.street;
        int info_id = get_info_id(player, street, bet_id), cluster = game.cluster[player][street];
        const Blueprint& blue = blueprint[biases[player]][player][street][info_id][cluster];

        int action_id = blue.get_sampled_action();
        bet_id = bet.children[action_id].bet_id;
    }
}

static inline float get_decision_value(
    const Gamestate& game,
    arr<int, 2> biases,
    int bet_id, int decider, int action_id, int learner, int num_rollouts
) {
    biases[decider] = action_id;    
    float value = 0;
    for (int i = 0; i < num_rollouts; i++)
        value += estimate_value(game, biases, bet_id, learner);
    value /= num_rollouts;
    return value;
}
static inline float get_frontier_value(
    const Gamestate& game,
    arr<int, 2> biases,
    int bet_id, int decider, int learner, int num_rollouts
) {
    const Betstate& bet = betstate[bet_id];
    int player = bet.player, street = bet.street;
    int info_id = get_info_id(player, street, bet_id), cluster = game.cluster[decider][street - 1];    
    Infoset& front = frontier[decider][info_id][cluster];
    front.upd_strat();

    float value = 0;
    if (decider == learner) {
        arr<float, MAX_NUM_ACTIONS> action_value;        
        for (int i = 0; i < front.num_actions; i++) {
            action_value[i] = get_decision_value(game, biases, bet_id, decider, i, learner, num_rollouts);
            value += action_value[i] * front.strat[i];
        }
        for (int i = 0; i < front.num_actions; i++) 
            front.cum_regret[i] += action_value[i] - value;   
    } else {
        int action_id = front.get_sampled_action();
        value = get_decision_value(game, biases, bet_id, decider, action_id, learner, num_rollouts);
        for (int i = 0; i < front.num_actions; i++) 
            front.cum_strat[i] += front.strat[i];
    }
    return value;
}

float get_value(
    const Gamestate& game,
    int bet_id, int learner, int start_street, int num_rollouts
) {  
    const Betstate& bet = betstate[bet_id];
    const Betstate::Payoff& payoff = bet.payoff;
    if (payoff.type != PayoffType::NonLeaf) 
        return get_leaf_value(payoff, game, learner);
    
    int player = bet.player, street = bet.street; 
    if (street != start_street) {
        arr<int, 2> biases = {};
        int decider = get_generator().get_random_bool();
        return get_frontier_value(game, biases, bet_id, decider, learner, num_rollouts);
    }
        
    int info_id = get_info_id(player, street, bet_id), cluster = game.cluster[player][street];
    Infoset& info = infoset[player][info_id][cluster];
    info.upd_strat();

    float value = 0;
    if (player == learner) {
        arr<float, MAX_NUM_ACTIONS> action_value;
        for (int i = 0; i < info.num_actions; i++) {
            int new_bet_id = bet.children[i].bet_id;
            action_value[i] = get_value(game, new_bet_id, learner, start_street, num_rollouts);
            value += action_value[i] * info.strat[i];
        }
        for (int i = 0; i < info.num_actions; i++)
            info.cum_regret[i] += action_value[i] - value;
    } else {
        int action_id = info.get_sampled_action();
        int new_bet_id = bet.children[action_id].bet_id;
        value = get_value(game, new_bet_id, learner, start_street, num_rollouts);
        for (int i = 0; i < info.num_actions; i++) 
            info.cum_strat[i] += info.strat[i];
    }
    return value;
}