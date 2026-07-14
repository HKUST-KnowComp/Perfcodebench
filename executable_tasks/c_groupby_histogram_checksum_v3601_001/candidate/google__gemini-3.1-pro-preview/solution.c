#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/**
 * Optimized dense histogram aggregation.
 * 
 * Improvements:
 * 1. Moved memory allocation (malloc) outside the 'iters' loop to avoid repeated syscalls.
 * 2. Replaced calloc with memset for faster buffer clearing.
 * 3. Unrolled the aggregation loop (factor of 8) to improve instruction-level parallelism (ILP).
 * 4. Ensured the checksum calculation remains bit-exact to the baseline.
 */
uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
    if (buckets == 0) return 0;

    // Allocate histogram buffer once outside the iteration loop
    uint64_t* hist = (uint64_t*)malloc(buckets * sizeof(uint64_t));
    if (!hist) return 0;

    uint64_t hash = 0;
    const uint64_t fnv_offset_basis = 1469598103934665603ULL;
    const uint64_t fnv_prime = 1099511628211ULL;

    for (int iter = 0; iter < iters; ++iter) {
        // Clear the histogram efficiently
        memset(hist, 0, buckets * sizeof(uint64_t));

        // Aggregate values into buckets with loop unrolling
        size_t i = 0;
        for (; i + 7 < n; i += 8) {
            hist[keys[i + 0]] += vals[i + 0];
            hist[keys[i + 1]] += vals[i + 1];
            hist[keys[i + 2]] += vals[i + 2];
            hist[keys[i + 3]] += vals[i + 3];
            hist[keys[i + 4]] += vals[i + 4];
            hist[keys[i + 5]] += vals[i + 5];
            hist[keys[i + 6]] += vals[i + 6];
            hist[keys[i + 7]] += vals[i + 7];
        }
        // Handle remaining elements
        for (; i < n; ++i) {
            hist[keys[i]] += vals[i];
        }

        // Calculate FNV-1a-style checksum
        hash = fnv_offset_basis;
        for (uint32_t j = 0; j < buckets; ++j) {
            hash ^= (hist[j] + 1ULL);
            hash *= fnv_prime;
        }
    }

    free(hist);
    return hash;
}
