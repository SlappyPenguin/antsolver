#pragma once

#include <bits/stdc++.h>
#include "solver.h"
using namespace std;

void init_clusters(arr<Map<short>, NUM_STREETS>& clusters, const arr<str, NUM_STREETS>& clusters_file);
void init_betstate(arr<Betstate, NUM_BETSTATES>& betstate, str betstates_file);
void init_blueprint(
    const arr<Betstate, NUM_BETSTATES>& betstate,                 
    arr<arr<arr<arr<vec<Blueprint>, MAX_BUCKET_SIZE>, NUM_STREETS>, 2>, NUM_BIASES>& blueprint,
    str blueprint_file
);
void init_search(
    const arr<Betstate, NUM_BETSTATES>& betstate, 
    arr<arr<arr<arr<vec<Blueprint>, MAX_BUCKET_SIZE>, NUM_STREETS>, 2>, NUM_BIASES>& blueprint,
    arr<arr<vec<Infoset>, MAX_BUCKET_SIZE>, 2>& infoset,
    arr<arr<vec<Infoset>, MAX_BUCKET_SIZE>, 2>& frontier,
    int bet_id
);
