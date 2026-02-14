
#include <bits/stdc++.h>
#include "../../include/solver.h"
#include "../../include/rank.h"
using namespace std;

const str RANKS_FILE = "../data/ranks.dat";

static int HR[32487834];
void init_ranks() {
    memset(HR, 0, sizeof(HR));
    FILE* file = fopen(RANKS_FILE.c_str(), "rb");
    fread(HR, sizeof(HR), 1, file);
    fclose(file);
}

static int get_hand_info(int* cards) {
    int p = HR[53 + *cards++];
    p = HR[p + *cards++];
    p = HR[p + *cards++];
    p = HR[p + *cards++];
    p = HR[p + *cards++];
    p = HR[p + *cards++];
    return HR[p + *cards++];
}

// Higher is better
// In total, there are 7462 equivalent hand ranks
int get_rank(arr<int, NUM_FINAL_CARDS> cards) {
    int cards_arr[NUM_FINAL_CARDS];
    for (int i = 0; i < NUM_FINAL_CARDS; i++) 
        cards_arr[i] = cards[i] + 1;
    int info = get_hand_info(cards_arr);
    return (info >> 12) * 8000 + (info & 0x00000FFF); 
}