#ifndef FRAMEWORK_PRNG_H
#define FRAMEWORK_PRNG_H

#include <cstdint>

struct XorShift64 {
    uint64_t state;

    explicit XorShift64(uint64_t seed)
        : state(seed ? seed : 88172645463325252ULL) {}

    inline uint64_t next() {
        uint64_t x = state;
        x ^= x << 13;
        x ^= x >> 7;
        x ^= x << 17;
        return state = x;
    }

    inline double uniform_double() {
        return (next() >> 11) * (1.0 / 9007199254740992.0);
    }

    inline int uniform_int(int lo, int hi) {
        return lo + static_cast<int>(uniform_double() * (hi - lo + 1.0));
    }
};

#endif