#pragma once

#include <bits/stdc++.h>
#include "solver.h"
using namespace std;

// Passing in becomes unwieldy quickly
extern arr<Betstate, NUM_BETSTATES> betstate;
extern arr<arr<arr<arr<vec<Blueprint>, MAX_BUCKET_SIZE>, NUM_STREETS>, 2>, NUM_BIASES> blueprint;
extern arr<arr<vec<Infoset>, MAX_BUCKET_SIZE>, 2> infoset;
extern arr<arr<vec<Infoset>, MAX_BUCKET_SIZE>, 2> frontier;
extern arr<Map<short>, NUM_STREETS> clusters;

void init_reach_prob();
void update_reach_prob(
    const arr<bool, NUM_CARDS>& in_public, 
    arr<int, NUM_FINAL_CARDS> cards, 
    int bet_id, int action
);
float get_reach_prob(arr<int, NUM_HOLE_CARDS> hole_cards, int player);
void search(
    const arr<int, NUM_FINAL_CARDS>& cards, 
    const arr<bool, NUM_CARDS>& in_public, 
    vec<int> remaining_cards, 
    int bet_id
);