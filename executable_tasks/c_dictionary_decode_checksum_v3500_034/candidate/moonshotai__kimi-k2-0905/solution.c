#include <stddef.h>
#include <stdint.h>
#include <emmintrin.h>

uint64_t run(const uint32_t *ids, size_t n, const uint32_t *dict_vals, uint32_t dict_size, int iters) {
    (void)dict_size;
    /* FNV-1a 64-bit constants */
    const uint64_t FNV_OFFSET = 1469598103934665603ULL;
    const uint64_t FNV_PRIME  = 1099511628211ULL;

    /* Four independent 64-bit accumulators for SSE2 unrolling */
    uint64_t h0, h1, h2, h3;

    /* Main benchmark loop */
    for (int iter = 0; iter < iters; ++iter) {
        h0 = h1 = h2 = h3 = FNV_OFFSET;

        size_t i = 0;
        /* Process 4 elements per iteration */
        for (; i + 3 < n; i += 4) {
            uint32_t v0 = dict_vals[ids[i+0]];
            uint32_t v1 = dict_vals[ids[i+1]];
            uint32_t v2 = dict_vals[ids[i+2]];
            uint32_t v3 = dict_vals[ids[i+3]];

            h0 ^= (uint64_t)v0 + 1ULL; h0 *= FNV_PRIME;
            h1 ^= (uint64_t)v1 + 1ULL; h1 *= FNV_PRIME;
            h2 ^= (uint64_t)v2 + 1ULL; h2 *= FNV_PRIME;
            h3 ^= (uint64_t)v3 + 1ULL; h3 *= FNV_PRIME;
        }
        /* Scalar tail */
        for (; i < n; ++i) {
            uint32_t v = dict_vals[ids[i]];
            h0 ^= (uint64_t)v + 1ULL;
            h0 *= FNV_PRIME;
        }
        /* Fold the four streams together */
        h0 ^= h1;
        h0 *= FNV_PRIME;
        h0 ^= h2;
        h0 *= FNV_PRIME;
        h0 ^= h3;
        h0 *= FNV_PRIME;
    }
    return h0;
}