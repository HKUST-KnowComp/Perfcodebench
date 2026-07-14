#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint64_t run(const uint32_t *__restrict keys,
             const uint32_t *__restrict vals,
             size_t n,
             uint32_t buckets,
             int iters)
{
    static const uint64_t prime = 1099511628211ULL;
    static const uint64_t offset = 1469598103934665603ULL;

    uint64_t *hist = (uint64_t *)calloc(buckets, sizeof(uint64_t));
    if (!hist) return 0;

    uint64_t hash = offset;
    for (int iter = 0; iter < iters; ++iter) {
        memset(hist, 0, buckets * sizeof(uint64_t));

        size_t i = 0;
        size_t limit = n & ~(size_t)7;
        for (; i < limit; i += 8) {
            hist[keys[i  ]] += vals[i  ];
            hist[keys[i+1]] += vals[i+1];
            hist[keys[i+2]] += vals[i+2];
            hist[keys[i+3]] += vals[i+3];
            hist[keys[i+4]] += vals[i+4];
            hist[keys[i+5]] += vals[i+5];
            hist[keys[i+6]] += vals[i+6];
            hist[keys[i+7]] += vals[i+7];
        }
        for (; i < n; ++i) hist[keys[i]] += vals[i];

        hash = offset;
        for (uint32_t b = 0; b < buckets; ++b) {
            hash ^= (hist[b] + 1ULL);
            hash *= prime;
        }
    }
    free(hist);
    return hash;
}