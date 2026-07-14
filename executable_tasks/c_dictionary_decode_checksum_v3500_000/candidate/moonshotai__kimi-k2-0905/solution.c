#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint32_t* ids, size_t n, const uint32_t* dict_vals, uint32_t dict_size, int iters) {
    (void)dict_size;
    const uint64_t prime = 1099511628211ULL;
    const uint64_t offset = 1469598103934665603ULL;

    uint64_t h = 0;
    for (int iter = 0; iter < iters; ++iter) {
        h = offset;
        size_t i = 0;
        /* peel tail in 8× unrolled blocks */
        for (; i + 7 < n; i += 8) {
            uint64_t v0 = dict_vals[ids[i  ]] + 1ULL;
            uint64_t v1 = dict_vals[ids[i+1]] + 1ULL;
            uint64_t v2 = dict_vals[ids[i+2]] + 1ULL;
            uint64_t v3 = dict_vals[ids[i+3]] + 1ULL;
            uint64_t v4 = dict_vals[ids[i+4]] + 1ULL;
            uint64_t v5 = dict_vals[ids[i+5]] + 1ULL;
            uint64_t v6 = dict_vals[ids[i+6]] + 1ULL;
            uint64_t v7 = dict_vals[ids[i+7]] + 1ULL;

            h ^= v0; h *= prime;
            h ^= v1; h *= prime;
            h ^= v2; h *= prime;
            h ^= v3; h *= prime;
            h ^= v4; h *= prime;
            h ^= v5; h *= prime;
            h ^= v6; h *= prime;
            h ^= v7; h *= prime;
        }
        /* handle remaining elements */
        for (; i < n; ++i) {
            h ^= (uint64_t)dict_vals[ids[i]] + 1ULL;
            h *= prime;
        }
    }
    return h;
}