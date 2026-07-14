#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
    uint64_t* hist = (uint64_t*)malloc((size_t)buckets * sizeof(uint64_t));
    if (!hist) return 0;

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        memset(hist, 0, (size_t)buckets * sizeof(uint64_t));

        for (size_t i = 0; i < n; ++i) {
            hist[keys[i]] += vals[i];
        }

        hash = 1469598103934665603ULL;
        uint32_t i = 0;
        for (; i + 3 < buckets; i += 4) {
            hash ^= (hist[i] + 1ULL); hash *= 1099511628211ULL;
            hash ^= (hist[i+1] + 1ULL); hash *= 1099511628211ULL;
            hash ^= (hist[i+2] + 1ULL); hash *= 1099511628211ULL;
            hash ^= (hist[i+3] + 1ULL); hash *= 1099511628211ULL;
        }
        for (; i < buckets; ++i) {
            hash ^= (hist[i] + 1ULL);
            hash *= 1099511628211ULL;
        }
    }
    free(hist);
    return hash;
}