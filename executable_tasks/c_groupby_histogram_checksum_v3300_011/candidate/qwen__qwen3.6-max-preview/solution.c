#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
    uint64_t* hist = (uint64_t*)malloc(buckets * sizeof(uint64_t));
    if (!hist) return 0;

    const uint32_t* restrict k = keys;
    const uint32_t* restrict v = vals;
    uint64_t* restrict h = hist;

    uint64_t hash = 0;
    size_t n_unroll = n & ~(size_t)7;

    for (int iter = 0; iter < iters; ++iter) {
        memset(h, 0, buckets * sizeof(uint64_t));

        size_t i = 0;
        for (; i < n_unroll; i += 8) {
            h[k[i]]   += v[i];
            h[k[i+1]] += v[i+1];
            h[k[i+2]] += v[i+2];
            h[k[i+3]] += v[i+3];
            h[k[i+4]] += v[i+4];
            h[k[i+5]] += v[i+5];
            h[k[i+6]] += v[i+6];
            h[k[i+7]] += v[i+7];
        }
        for (; i < n; ++i) {
            h[k[i]] += v[i];
        }

        hash = 1469598103934665603ULL;
        for (uint32_t b = 0; b < buckets; ++b) {
            hash ^= (h[b] + 1ULL);
            hash *= 1099511628211ULL;
        }
    }

    free(hist);
    return hash;
}