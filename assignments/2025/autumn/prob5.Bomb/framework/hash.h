#ifndef FRAMEWORK_HASH_H
#define FRAMEWORK_HASH_H

#include <cstddef>
#include <cstdint>

static inline uint64_t fnv1a64(const uint8_t* data, size_t len) {
    constexpr uint64_t FNV_PRIME = 1099511628211ULL;
    constexpr uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;

    uint64_t hash = FNV_OFFSET_BASIS;
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= FNV_PRIME;
    }
    return hash;
}


#endif