#pragma once

#include <stdint.h>

static const double pi = 3.1415926535897932385;

void random_seed(uint64_t seed);

// Returns a random real in [0,1).
double random_double(void);

static inline double random_double_in(double min, double max) {
    // Returns a random real in [min,max).
    return min + (max - min) * random_double();
}

static inline int random_int(int min, int max) {
    // Returns a random integer in [min,max].
    return random_double_in(min, max + 1);
}

static inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180;
}

static inline double clamp(double x, double min, double max) {
    if (x < min)
        return min;
    if (x > max)
        return max;
    return x;
}
