/*
Converts any pairing of hole and board cards, on any street, into a valid card set (as outlined in sets.cpp).
*/

#include <bits/stdc++.h>
#include "../../include/solver.h"
#include "../../include/convert.h"
using namespace std;

static void sort_preflop_and_flop(arr<int, NUM_FINAL_CARDS>& cards, int street) {
    sort(cards.begin(), cards.begin() + CUM_STREET_SIZE[0]);
    if (street == 0) return;
    sort(cards.begin() + CUM_STREET_SIZE[0], cards.begin() + CUM_STREET_SIZE[1]);
}
arr<int, NUM_FINAL_CARDS> convert(arr<int, NUM_FINAL_CARDS> cards, int street) {
    sort_preflop_and_flop(cards, street);

    arr<int, NUM_FINAL_CARDS> rank, suit;
    arr<int, NUM_SUITS> suit_map;
    suit_map.fill(-1);
    int num_mapped = 0;
    for (int i = 0; i < CUM_STREET_SIZE[street]; i++) {
        rank[i] = get_rank(cards[i]), suit[i] = get_suit(cards[i]);
        if (suit_map[suit[i]] == -1) {
            suit_map[suit[i]] = num_mapped;
            num_mapped++;
        }
        suit[i] = suit_map[suit[i]];
    }

    arr<vec<pair<int, int>>, NUM_SUITS> appearances; 
    iota(suit_map.begin(), suit_map.end(), 0);
    for (int i = 0; i < CUM_STREET_SIZE[street]; i++) {        
        suit[i] = suit_map[suit[i]];
        
        int old_suit = suit[i];
        for (int other_suit = suit[i] - 1; other_suit >= 0; other_suit--)
            if (appearances[other_suit] == appearances[suit[i]])
                suit[i] = other_suit;
        if (old_suit != suit[i])
            suit_map[old_suit] = suit[i], suit_map[suit[i]] = old_suit;
        
        cards[i] = get_card(rank[i], suit[i]);
        appearances[suit[i]].push_back({rank[i], STREET_AT[i]});
    }

    sort_preflop_and_flop(cards, street);
    return cards;
}