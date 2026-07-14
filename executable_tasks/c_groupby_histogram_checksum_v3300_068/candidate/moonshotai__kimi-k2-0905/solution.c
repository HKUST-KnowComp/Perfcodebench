#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint64_t run(const uint32_t *restrict keys,
             const uint32_t *restrict vals,
             size_t n,
             uint32_t buckets,
             int iters) {
    /* one calloc, reused every iteration */
    uint64_t *restrict hist = (uint64_t *)aligned_alloc(64, buckets * sizeof(uint64_t));
    if (!hist) return 0;

    const uint64_t seed  = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        /* clear histogram quickly */
        memset(hist, 0, buckets * sizeof(uint64_t));

        /* aggregation phase */
        for (size_t i = 0; i < n; ++i)
            hist[keys[i]] += vals[i];

        /* hashing phase – unroll 4× */
        hash = seed;
        uint32_t i = 0;
        for (; i + 4 <= buckets; i += 4) {
            hash ^= (hist[i]   + 1); hash *= prime;
            hash ^= (hist[i+1] + 1); hash *= prime;
            hash ^= (hist[i+2] + 1); hash *= prime;
            hash ^= (hist[i+3] + 1); hash *= prime;
        }
        for (; i < buckets; ++i) {
            hash ^= (hist[i] + 1);
            hash *= prime;
        }
    }

    free(hist);
    return hash;
}