#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/**
 * Optimized dense histogram aggregation.
 * 
 * Improvements:
 * 1. Moved memory allocation outside the iteration loop to avoid repeated malloc/free overhead.
 * 2. Used memset to clear the histogram buffer efficiently.
 * 3. Unrolled the accumulation loop by a factor of 8 to improve ILP (Instruction Level Parallelism).
 * 4. Used a local pointer for the histogram to assist the compiler with alias analysis.
 */
uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
    if (buckets == 0) return 0;

    uint64_t* hist = (uint64_t*)malloc(buckets * sizeof(uint64_t));
    if (!hist) return 0;

    uint64_t hash = 0;
    const uint64_t fnv_offset_basis = 1469598103934665603ULL;
    const uint64_t fnv_prime = 1099511628211ULL;

    for (int iter = 0; iter < iters; ++iter) {
        memset(hist, 0, buckets * sizeof(uint64_t));

        size_t i = 0;
        // Unroll the accumulation loop by 8
        for (; i + 7 < n; i += 8) {
            uint32_t k0 = keys[i], k1 = keys[i+1], k2 = keys[i+2], k3 = keys[i+3];
            uint32_t k4 = keys[i+4], k5 = keys[i+5], k6 = keys[i+6], k7 = keys[i+7];
            uint32_t v0 = vals[i], v1 = vals[i+1], v2 = vals[i+2], v3 = vals[i+3];
            uint32_t v4 = vals[i+4], v5 = vals[i+5], v6 = vals[i+6], v7 = vals[i+7];
            
            hist[k0] += v0;
            hist[k1] += v1;
            hist[k2] += v2;
            hist[k3] += v3;
            hist[k4] += v4;
            hist[k5] += v5;
            hist[k6] += v6;
            hist[k7] += v7;
        }
        // Handle remainder
        for (; i < n; ++i) {
            hist[keys[i]] += vals[i];
        }

        // FNV-1a checksum calculation
        hash = fnv_offset_basis;
        for (uint32_t j = 0; j < buckets; ++j) {
            hash ^= (hist[j] + 1ULL);
            hash *= fnv_prime;
        }
    }

    free(hist);
    return hash;
}
