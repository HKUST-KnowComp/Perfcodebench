#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint32_t* restrict keys, const uint32_t* restrict vals, size_t n, uint32_t buckets, int iters) {
    uint64_t* hist = (uint64_t*)calloc(buckets, sizeof(uint64_t));
    if (!hist) return 0;

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        size_t i = 0;
        size_t limit = (n >> 2) << 2;
        for (; i < limit; i += 4) {
            hist[keys[i]]     += vals[i];
            hist[keys[i + 1]] += vals[i + 1];
            hist[keys[i + 2]] += vals[i + 2];
            hist[keys[i + 3]] += vals[i + 3];
        }
        for (; i < n; ++i) {
            hist[keys[i]] += vals[i];
        }

        hash = 1469598103934665603ULL;
        for (uint32_t b = 0; b < buckets; ++b) {
            uint64_t v = hist[b];
            hash ^= (v + 1ULL);
            hash *= 1099511628211ULL;
            hist[b] = 0;
        }
    }

    free(hist);
    return hash;
}