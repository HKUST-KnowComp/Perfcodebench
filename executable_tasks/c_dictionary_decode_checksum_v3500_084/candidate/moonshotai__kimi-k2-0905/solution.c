#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint32_t* ids, size_t n, const uint32_t* dict_vals, uint32_t dict_size, int iters) {
    (void)dict_size;
    const uint64_t prime = 1099511628211ULL;
    const uint64_t seed  = 1469598103934665603ULL;
    uint64_t h = seed;

    for (int iter = 0; iter < iters; ++iter) {
        h = seed;
        size_t i = 0;

        /* 4-way unrolled inner loop */
        for (; i + 3 < n; i += 4) {
            uint32_t v0 = dict_vals[ids[i + 0]];
            uint32_t v1 = dict_vals[ids[i + 1]];
            uint32_t v2 = dict_vals[ids[i + 2]];
            uint32_t v3 = dict_vals[ids[i + 3]];

            h ^= (uint64_t)v0 + 1ULL; h *= prime;
            h ^= (uint64_t)v1 + 1ULL; h *= prime;
            h ^= (uint64_t)v2 + 1ULL; h *= prime;
            h ^= (uint64_t)v3 + 1ULL; h *= prime;
        }

        /* tail */
        for (; i < n; ++i) {
            uint32_t v = dict_vals[ids[i]];
            h ^= (uint64_t)v + 1ULL;
            h *= prime;
        }
    }
    return h;
}