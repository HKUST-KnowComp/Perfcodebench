#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

/**
 * Optimized dictionary decode and checksum calculation.
 * 
 * Performance improvements:
 * 1. Removed intermediate memory allocation (malloc/free) inside the function.
 * 2. Combined decoding and hashing into a single pass to reduce memory bandwidth usage.
 * 3. Unrolled the loop to improve instruction-level parallelism (ILP).
 * 4. Used the FNV-1a constants directly in the loop.
 */
uint64_t run(const uint32_t* ids, size_t n, const uint32_t* dict_vals, uint32_t dict_size, int iters) {
    (void)dict_size;
    uint64_t h = 0;
    const uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
    const uint64_t FNV_PRIME = 1099511628211ULL;

    for (int iter = 0; iter < iters; ++iter) {
        h = FNV_OFFSET_BASIS;
        size_t i = 0;

        // Unroll loop by 4 to improve ILP and reduce branch overhead
        for (; i + 3 < n; i += 4) {
            uint32_t v0 = dict_vals[ids[i]];
            uint32_t v1 = dict_vals[ids[i + 1]];
            uint32_t v2 = dict_vals[ids[i + 2]];
            uint32_t v3 = dict_vals[ids[i + 3]];

            h ^= (uint64_t)v0 + 1ULL;
            h *= FNV_PRIME;
            h ^= (uint64_t)v1 + 1ULL;
            h *= FNV_PRIME;
            h ^= (uint64_t)v2 + 1ULL;
            h *= FNV_PRIME;
            h ^= (uint64_t)v3 + 1ULL;
            h *= FNV_PRIME;
        }

        // Handle remaining elements
        for (; i < n; ++i) {
            uint32_t v = dict_vals[ids[i]];
            h ^= (uint64_t)v + 1ULL;
            h *= FNV_PRIME;
        }
    }

    return h;
}