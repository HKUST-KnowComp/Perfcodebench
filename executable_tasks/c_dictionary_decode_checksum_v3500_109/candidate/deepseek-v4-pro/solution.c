#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint32_t* restrict ids, size_t n, const uint32_t* restrict dict_vals, uint32_t dict_size, int iters) {
    (void)dict_size;
    if (iters <= 0) return 0;
    
    const uint64_t multiplier = 1099511628211ULL;
    const uint64_t seed = 1469598103934665603ULL;
    uint64_t h = seed;
    
    for (int iter = 0; iter < iters; ++iter) {
        h = seed;
        size_t i = 0;
        size_t limit = n - (n % 4);
        while (i < limit) {
            uint32_t v0 = dict_vals[ids[i]];
            uint32_t v1 = dict_vals[ids[i+1]];
            uint32_t v2 = dict_vals[ids[i+2]];
            uint32_t v3 = dict_vals[ids[i+3]];
            h ^= (uint64_t)v0 + 1ULL;
            h *= multiplier;
            h ^= (uint64_t)v1 + 1ULL;
            h *= multiplier;
            h ^= (uint64_t)v2 + 1ULL;
            h *= multiplier;
            h ^= (uint64_t)v3 + 1ULL;
            h *= multiplier;
            i += 4;
        }
        while (i < n) {
            h ^= (uint64_t)dict_vals[ids[i]] + 1ULL;
            h *= multiplier;
            ++i;
        }
    }
    return h;
}
