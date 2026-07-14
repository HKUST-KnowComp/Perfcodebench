#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/**
 * Optimized C dense histogram aggregation.
 * 
 * Improvements:
 * 1. Moved memory allocation (malloc) outside the iteration loop to avoid repeated syscalls.
 * 2. Replaced calloc with memset for faster buffer clearing.
 * 3. Unrolled the histogram accumulation loop (factor of 8) to improve ILP (Instruction Level Parallelism)
 *    and allow the CPU to overlap memory loads for keys and values.
 * 4. Used a local pointer for the histogram to assist the compiler with alias analysis.
 */
uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
    if (buckets == 0) return 0;

    uint64_t* hist = (uint64_t*)malloc(buckets * sizeof(uint64_t));
    if (!hist) return 0;

    uint64_t hash = 0;
    const uint64_t fnv_offset = 1469598103934665603ULL;
    const uint64_t fnv_prime = 1099511628211ULL;

    for (int iter = 0; iter < iters; ++iter) {
        memset(hist, 0, buckets * sizeof(uint64_t));

        size_t i = 0;
        // Unroll the accumulation loop to process 8 elements at a time
        for (; i + 7 < n; i += 8) {
            uint32_t k0 = keys[i], v0 = vals[i];
            uint32_t k1 = keys[i+1], v1 = vals[i+1];
            uint32_t k2 = keys[i+2], v2 = vals[i+2];
            uint32_t k3 = keys[i+3], v3 = vals[i+3];
            uint32_t k4 = keys[i+4], v4 = vals[i+4];
            uint32_t k5 = keys[i+5], v5 = vals[i+5];
            uint32_t k6 = keys[i+6], v6 = vals[i+6];
            uint32_t k7 = keys[i+7], v7 = vals[i+7];

            hist[k0] += v0;
            hist[k1] += v1;
            hist[k2] += v2;
            hist[k3] += v3;
            hist[k4] += v4;
            hist[k5] += v5;
            hist[k6] += v6;
            hist[k7] += v7;
        }

        // Handle remaining elements
        for (; i < n; ++i) {
            hist[keys[i]] += vals[i];
        }

        // Checksum calculation (FNV-1a style)
        hash = fnv_offset;
        for (uint32_t j = 0; j < buckets; ++j) {
            hash ^= (hist[j] + 1ULL);
            hash *= fnv_prime;
        }
    }

    free(hist);
    return hash;
}
