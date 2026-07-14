#include "interface.h"
#include <stdint.h>

uint64_t run(const uint32_t* restrict ids, const uint32_t* restrict ts, const uint16_t* restrict levels, const uint16_t* restrict lens, size_t count, int iters) {
    const uint64_t initial_hash = 1469598103934665603ULL;
    const uint64_t multiplier = 1099511628211ULL;
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t h = initial_hash;
        for (size_t i = 0; i < count; ++i) {
            uint32_t id = ids[i];
            uint32_t t = ts[i];
            uint16_t lvl = levels[i];
            uint16_t len_ = lens[i];

            h ^= (uint64_t)(id & 0xFFu);
            h *= multiplier;
            h ^= (uint64_t)((id >> 8) & 0xFFu);
            h *= multiplier;
            h ^= (uint64_t)((id >> 16) & 0xFFu);
            h *= multiplier;
            h ^= (uint64_t)((id >> 24) & 0xFFu);
            h *= multiplier;

            h ^= (uint64_t)(t & 0xFFu);
            h *= multiplier;
            h ^= (uint64_t)((t >> 8) & 0xFFu);
            h *= multiplier;
            h ^= (uint64_t)((t >> 16) & 0xFFu);
            h *= multiplier;
            h ^= (uint64_t)((t >> 24) & 0xFFu);
            h *= multiplier;

            h ^= (uint64_t)(lvl & 0xFFu);
            h *= multiplier;
            h ^= (uint64_t)((lvl >> 8) & 0xFFu);
            h *= multiplier;

            h ^= (uint64_t)(len_ & 0xFFu);
            h *= multiplier;
            h ^= (uint64_t)((len_ >> 8) & 0xFFu);
            h *= multiplier;
        }
        hash = h;
    }
    return hash;
}
