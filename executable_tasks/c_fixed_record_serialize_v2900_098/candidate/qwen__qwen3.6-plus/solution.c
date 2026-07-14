#include "interface.h"
#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint32_t* ids, const uint32_t* ts, const uint16_t* levels, const uint16_t* lens, size_t count, int iters) {
    size_t total = count * 12;
    uint8_t* out = (uint8_t*)malloc(total);
    if (!out) exit(4);

    uint64_t final_hash = 0;
    const uint64_t FNV_OFFSET = 1469598103934665603ULL;
    const uint64_t FNV_PRIME = 1099511628211ULL;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t h = FNV_OFFSET;
        uint8_t* p = out;
        for (size_t i = 0; i < count; ++i) {
            uint32_t id = ids[i];
            uint32_t t = ts[i];
            uint16_t l = levels[i];
            uint16_t ln = lens[i];

            h ^= (uint8_t)id; h *= FNV_PRIME; *p++ = (uint8_t)id;
            h ^= (uint8_t)(id >> 8); h *= FNV_PRIME; *p++ = (uint8_t)(id >> 8);
            h ^= (uint8_t)(id >> 16); h *= FNV_PRIME; *p++ = (uint8_t)(id >> 16);
            h ^= (uint8_t)(id >> 24); h *= FNV_PRIME; *p++ = (uint8_t)(id >> 24);

            h ^= (uint8_t)t; h *= FNV_PRIME; *p++ = (uint8_t)t;
            h ^= (uint8_t)(t >> 8); h *= FNV_PRIME; *p++ = (uint8_t)(t >> 8);
            h ^= (uint8_t)(t >> 16); h *= FNV_PRIME; *p++ = (uint8_t)(t >> 16);
            h ^= (uint8_t)(t >> 24); h *= FNV_PRIME; *p++ = (uint8_t)(t >> 24);

            h ^= (uint8_t)l; h *= FNV_PRIME; *p++ = (uint8_t)l;
            h ^= (uint8_t)(l >> 8); h *= FNV_PRIME; *p++ = (uint8_t)(l >> 8);

            h ^= (uint8_t)ln; h *= FNV_PRIME; *p++ = (uint8_t)ln;
            h ^= (uint8_t)(ln >> 8); h *= FNV_PRIME; *p++ = (uint8_t)(ln >> 8);
        }
        final_hash = h;
    }

    free(out);
    return final_hash;
}
