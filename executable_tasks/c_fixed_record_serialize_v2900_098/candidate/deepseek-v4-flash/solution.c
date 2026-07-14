#include "interface.h"
#include <stdint.h>

uint64_t run(const uint32_t* ids, const uint32_t* ts, const uint16_t* levels, const uint16_t* lens, size_t count, int iters) {
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (size_t i = 0; i < count; ++i) {
            uint32_t id = ids[i];
            hash ^= (uint8_t)id;
            hash *= 1099511628211ULL;
            hash ^= (uint8_t)(id >> 8);
            hash *= 1099511628211ULL;
            hash ^= (uint8_t)(id >> 16);
            hash *= 1099511628211ULL;
            hash ^= (uint8_t)(id >> 24);
            hash *= 1099511628211ULL;
            uint32_t t = ts[i];
            hash ^= (uint8_t)t;
            hash *= 1099511628211ULL;
            hash ^= (uint8_t)(t >> 8);
            hash *= 1099511628211ULL;
            hash ^= (uint8_t)(t >> 16);
            hash *= 1099511628211ULL;
            hash ^= (uint8_t)(t >> 24);
            hash *= 1099511628211ULL;
            uint16_t lvl = levels[i];
            hash ^= (uint8_t)lvl;
            hash *= 1099511628211ULL;
            hash ^= (uint8_t)(lvl >> 8);
            hash *= 1099511628211ULL;
            uint16_t len = lens[i];
            hash ^= (uint8_t)len;
            hash *= 1099511628211ULL;
            hash ^= (uint8_t)(len >> 8);
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}