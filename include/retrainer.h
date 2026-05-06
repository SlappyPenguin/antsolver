#pragma once

#include "solver.h"
using namespace std;

extern arr<Betstate, NUM_BETSTATES> betstate;
extern arr<arr<arr<arr<vec<Blueprint>, MAX_BUCKET_SIZE>, NUM_STREETS>, 2>, NUM_BIASES> blueprint;
extern arr<arr<vec<Infoset>, MAX_BUCKET_SIZE>, 2> infoset;
extern arr<arr<vec<Infoset>, MAX_BUCKET_SIZE>, 2> frontier;

float get_value(
    const Gamestate& game,
    int bet_id, int learner, int start_street, int num_rollouts
);