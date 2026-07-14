#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
    uint64_t* hist = (uint64_t*)calloc(buckets, sizeof(uint64_t));
    if (!hist) return 0;

    uint64_t hash = 0;
    size_t n_lim = n & ~(size_t)3;

    for (int iter = 0; iter < iters; ++iter) {
        memset(hist, 0, buckets * sizeof(uint64_t));

        size_t i = 0;
        for (; i < n_lim; i += 4) {
            hist[keys[i]]   += vals[i];
            hist[keys[i+1]] += vals[i+1];
            hist[keys[i+2]] += vals[i+2];
            hist[keys[i+3]] += vals[i+3];
        }
        for (; i < n; ++i) {
            hist[keys[i]] += vals[i];
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
