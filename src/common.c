#include "common.h"

// https://prng.di.unimi.it/splitmix64.c

static uint64_t splitmix64_next(uint64_t* x) {
    uint64_t z = (*x += 0x9e3779b97f4a7c15);
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
    z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
    return z ^ (z >> 31);
}

// https://prng.di.unimi.it/xoshiro256plus.c

#define STATE_SIZE 4

static inline uint64_t rotl(const uint64_t x, int k) {
    return (x << k) | (x >> (64 - k));
}

static uint64_t s[STATE_SIZE];

static uint64_t xoshiro256plus_next(void) {
    const uint64_t result = s[0] + s[3];

    const uint64_t t = s[1] << 17;

    s[2] ^= s[0];
    s[3] ^= s[1];
    s[1] ^= s[2];
    s[0] ^= s[3];

    s[2] ^= t;

    s[3] = rotl(s[3], 45);

    return result;
}

void random_seed(uint64_t seed) {
    for (int i = 0; i < STATE_SIZE; ++i)
        s[i] = splitmix64_next(&seed);
}

double random_double(void) { return (xoshiro256plus_next() >> 11) * 0x1.0p-53; }
