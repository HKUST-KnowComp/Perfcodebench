#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
    uint64_t hash = 0;
    uint64_t* hist = (uint64_t*)calloc(buckets, sizeof(uint64_t));
    if (!hist) return 0;

    for (int iter = 0; iter < iters; ++iter) {
        __builtin_memset(hist, 0, buckets * sizeof(uint64_t));

        uint64_t* restrict h = hist;
        for (size_t i = 0; i < n; ++i) {
            h[keys[i]] += vals[i];
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