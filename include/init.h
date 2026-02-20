#pragma once

#include <bits/stdc++.h>
#include "solver.h"
using namespace std;

void init_clusters(arr<Map<short>, NUM_STREETS>& clusters);
void init_betstate(arr<Betstate, NUM_BETSTATES>& betstate);
void init_blueprint(
    const arr<Betstate, NUM_BETSTATES>& betstate,                 
    arr<arr<arr<arr<vec<Blueprint>, MAX_BUCKET_SIZE>, NUM_STREETS>, 2>, NUM_BIASES>& blueprint
);
void init_infoset(
    const arr<Betstate, NUM_BETSTATES>& betstate,
    arr<arr<arr<vec<Infoset>, MAX_BUCKET_SIZE>, NUM_STREETS>, 2>& infoset
);
void init_search(
    const arr<Betstate, NUM_BETSTATES>& betstate, 
    arr<arr<arr<arr<vec<Blueprint>, MAX_BUCKET_SIZE>, NUM_STREETS>, 2>, NUM_BIASES>& blueprint,
    arr<arr<vec<Infoset>, MAX_BUCKET_SIZE>, 2>& infoset,
    arr<arr<vec<Infoset>, MAX_BUCKET_SIZE>, 2>& frontier,
    int bet_id
);
