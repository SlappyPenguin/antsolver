/*
Generates all card sets in Poker which are strategically distinct, across every round of play. A card set is all 
the card information available to a player at any time. This includes their hole cards and all public cards so far. 
The id of a card = 4 * rank + suit - 8 where rank is in [2, 14] and suit is in [0, 3].

In order to filter out strategically identical card sets, apply the following rules:

1) An arbitrary order of suits (c>d>h>s) is applied, such that every next card is either the next unused suit in 
   the suit order or a previously used suit
        e.g. 2c3c 5c/d (not 5h)

2) For the cards on the preflop and flop, they are sorted in increasing order of id
        e.g. 2d4c KhKsAc (not KsKhAc)

3) When a new card is assigned a previously used suit, the suit must be the lowest suit with its specific 
   appearances in rank and street (superset of first rule)
        e.g. 5c5d 6c (not 6d)
             4c5c 8d8h8sTd (not Th)

If we filter out strategically identical sets, there are 169 at the preflop, 1,286,792 at the flop, 55,190,538 at 
the turn, and 2,428,287,420 at the river. 

Solution verified from https://noambrown.github.io/papers/19-Science-Superhuman_Supp.pdf.
*/

#include <bits/stdc++.h>
#include "../../include/solver.h"
using namespace std;

const arr<str, NUM_STREETS> OUTPUT_FILES = {
    "../data/preflop_sets.bin", 
    "../data/flop_sets.bin", 
    "../data/turn_sets.bin", 
    "../data/river_sets.bin"
};

struct Set {
    arr<int, NUM_FINAL_CARDS> cards = {};
    arr<vec<pair<int, int>>, NUM_SUITS> appearances;
    void print(ofstream& file, int street) const {
        lint id = get_id(cards, street);
        write(file, id);
    }
};

vec<Set> add_card(vec<Set> sets, int index, int street_end_at) {
    vec<Set> ans;
    ofstream file;
    if (street_end_at != -1) file.open(OUTPUT_FILES[street_end_at], ios::binary);
    for (Set set : sets) {
        for (int card = 0; card < NUM_CARDS; card++) {
            // No duplicate cards
            if (count(set.cards.begin(), set.cards.begin() + index, card) > 0) continue;
            
            // Preflop and flop must be sorted
            bool on_preflop = (index > 0 && STREET_AT[index] == (int) Street::Preflop);
            bool on_flop = (index > NUM_HOLE_CARDS && STREET_AT[index] == (int) Street::Flop);
            if ((on_preflop || on_flop) && set.cards[index - 1] >= card) continue;
            
            // Use lowest suit with certain set of appearances
            int rank = get_rank(card), suit = get_suit(card);
            bool skip = false;
            for (int other_suit = suit - 1; other_suit >= 0; other_suit--)
                skip |= (set.appearances[suit] == set.appearances[other_suit]);
            if (skip) continue;

            // Add card to set
            Set new_set = set;
            new_set.cards[index] = card;
            new_set.appearances[suit].push_back({rank, STREET_AT[index]});

            if (street_end_at != -1) new_set.print(file, street_end_at);
            ans.push_back(new_set);
        }
    }
    return ans;
}

int main() {    
    vec<Set> sets = {{}};
    for (int i = 0; i < 5; i++) {
        int street_end_at = -1;
        for (int j = 0; j < NUM_STREETS; j++) {
            if (i != CUM_STREET_SIZE[j] - 1) continue;
            street_end_at = j;  
            break;
        }
        sets = add_card(sets, i, street_end_at);
    }
}