/*
Implements xorshift64 from https://en.wikipedia.org/wiki/Xorshift for a fast random number generator. Randomness 
could be improved by using a more complex generator, but this is enough and very fast.
*/

#pragma once

#include <bits/stdc++.h>
using namespace std;

struct Generator {
    uint64_t state;
    Generator() {
        random_device dev;
        state = dev();
    }
    inline uint64_t get_next_state() {
        uint64_t x = state;
        x ^= x << 13;
        x ^= x >> 7;
        x ^= x << 17;
        return state = x;
    }
    inline float get_random_float() {
        return (get_next_state() >> 11) * (1.0f / (1ULL << 53));
    }
    inline bool get_random_bool() {
        return get_next_state() >> 63;
    }
    inline int get_random_int(int left, int right) {
        int range = right - left + 1;
        return left + (int) (get_next_state() % range);
    }
};
inline Generator& get_generator() {
    thread_local Generator gen;
    return gen;
}
inline mt19937& get_slow_generator() {
    thread_local random_device dev;
    thread_local mt19937 gen(dev());
    return gen;
}