#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/**
 * Optimized implementation of dense histogram aggregation.
 * Improvements:
 * 1. Moved memory allocation (malloc) outside the iteration loop to avoid repeated syscalls.
 * 2. Used memset to clear the histogram buffer efficiently.
 * 3. Unrolled the histogram accumulation loop (4x) to improve throughput.
 * 4. Unrolled the FNV-1a hashing loop (4x) to reduce branch overhead.
 * 5. Used __builtin_assume_aligned or standard pointers with manual unrolling for performance.
 */
uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
    if (buckets == 0) return 0;

    uint64_t* hist = (uint64_t*)malloc(buckets * sizeof(uint64_t));
    if (!hist) return 0;

    uint64_t hash = 0;
    const uint64_t fnv_offset = 1469598103934665603ULL;
    const uint64_t fnv_prime = 1099511628211ULL;

    for (int iter = 0; iter < iters; ++iter) {
        // Clear histogram
        memset(hist, 0, buckets * sizeof(uint64_t));

        // Accumulate values into histogram buckets
        size_t i = 0;
        for (; i + 3 < n; i += 4) {
            uint32_t k0 = keys[i];
            uint32_t v0 = vals[i];
            uint32_t k1 = keys[i + 1];
            uint32_t v1 = vals[i + 1];
            uint32_t k2 = keys[i + 2];
            uint32_t v2 = vals[i + 2];
            uint32_t k3 = keys[i + 3];
            uint32_t v3 = vals[i + 3];

            hist[k0] += v0;
            hist[k1] += v1;
            hist[k2] += v2;
            hist[k3] += v3;
        }
        for (; i < n; ++i) {
            hist[keys[i]] += vals[i];
        }

        // Compute FNV-1a hash over the histogram
        hash = fnv_offset;
        uint32_t b = 0;
        for (; b + 3 < buckets; b += 4) {
            hash ^= (hist[b] + 1ULL);
            hash *= fnv_prime;
            hash ^= (hist[b + 1] + 1ULL);
            hash *= fnv_prime;
            hash ^= (hist[b + 2] + 1ULL);
            hash *= fnv_prime;
            hash ^= (hist[b + 3] + 1ULL);
            hash *= fnv_prime;
        }
        for (; b < buckets; ++b) {
            hash ^= (hist[b] + 1ULL);
            hash *= fnv_prime;
        }
    }

    free(hist);
    return hash;
}
