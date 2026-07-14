#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
    uint64_t* hist = (uint64_t*)malloc(buckets * sizeof(uint64_t));
    if (!hist) return 0;

    uint64_t hash = 0;
    const uint32_t* __restrict__ k = keys;
    const uint32_t* __restrict__ v = vals;

    for (int iter = 0; iter < iters; ++iter) {
        memset(hist, 0, buckets * sizeof(uint64_t));

        size_t i = 0;
        for (; i + 3 < n; i += 4) {
            hist[k[i]]   += v[i];
            hist[k[i+1]] += v[i+1];
            hist[k[i+2]] += v[i+2];
            hist[k[i+3]] += v[i+3];
        }
        for (; i < n; ++i) {
            hist[k[i]] += v[i];
        }

        hash = 1469598103934665603ULL;
        for (uint32_t j = 0; j < buckets; ++j) {
            hash ^= (hist[j] + 1ULL);
            hash *= 1099511628211ULL;
        }
    }

    free(hist);
    return hash;
}
