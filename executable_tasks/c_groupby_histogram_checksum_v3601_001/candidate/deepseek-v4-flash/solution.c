#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
    // Handle trivial case
    if (iters <= 0) return 0;

    // Allocate histogram once
    uint64_t* hist = (uint64_t*)calloc(buckets, sizeof(uint64_t));
    if (!hist) return 0;

    // Single pass: aggregate all keys and values
    for (size_t i = 0; i < n; ++i) {
        uint32_t k = keys[i];
        hist[k] += vals[i];
    }

    // Repeatedly compute FNV-1a hash over the histogram
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (uint32_t i = 0; i < buckets; ++i) {
            hash ^= (hist[i] + 1ULL);
            hash *= 1099511628211ULL;
        }
    }

    free(hist);
    return hash;
}
