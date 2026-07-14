#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
    if (buckets == 0) return 1469598103934665603ULL;
    uint64_t* hist = (uint64_t*)malloc((size_t)buckets * sizeof(uint64_t));
    if (!hist) return 0;

    uint64_t hash = 0;
    const uint32_t* restrict k = keys;
    const uint32_t* restrict v = vals;
    uint64_t* restrict h = hist;

    for (int iter = 0; iter < iters; ++iter) {
        memset(h, 0, (size_t)buckets * sizeof(uint64_t));

        size_t i = 0;
        for (; i + 3 < n; i += 4) {
            h[k[i]] += v[i];
            h[k[i+1]] += v[i+1];
            h[k[i+2]] += v[i+2];
            h[k[i+3]] += v[i+3];
        }
        for (; i < n; ++i) {
            h[k[i]] += v[i];
        }

        hash = 1469598103934665603ULL;
        for (uint32_t i = 0; i < buckets; ++i) {
            hash ^= (h[i] + 1ULL);
            hash *= 1099511628211ULL;
        }
    }

    free(hist);
    return hash;
}