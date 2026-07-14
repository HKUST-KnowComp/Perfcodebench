#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
    uint64_t* hist = (uint64_t*)malloc((size_t)buckets * sizeof(uint64_t));
    if (!hist) return 0;

    uint64_t hash = 0;
    const uint64_t FNV_PRIME = 1099511628211ULL;
    const uint64_t FNV_OFFSET = 1469598103934665603ULL;

    const uint32_t* __restrict k = keys;
    const uint32_t* __restrict v = vals;
    uint64_t* __restrict h = hist;

    for (int iter = 0; iter < iters; ++iter) {
        memset(h, 0, (size_t)buckets * sizeof(uint64_t));

        for (size_t i = 0; i < n; ++i) {
            h[k[i]] += v[i];
        }

        hash = FNV_OFFSET;
        uint32_t i = 0;
        for (; i + 3 < buckets; i += 4) {
            hash ^= (h[i] + 1ULL); hash *= FNV_PRIME;
            hash ^= (h[i+1] + 1ULL); hash *= FNV_PRIME;
            hash ^= (h[i+2] + 1ULL); hash *= FNV_PRIME;
            hash ^= (h[i+3] + 1ULL); hash *= FNV_PRIME;
        }
        for (; i < buckets; ++i) {
            hash ^= (h[i] + 1ULL); hash *= FNV_PRIME;
        }
    }

    free(hist);
    return hash;
}