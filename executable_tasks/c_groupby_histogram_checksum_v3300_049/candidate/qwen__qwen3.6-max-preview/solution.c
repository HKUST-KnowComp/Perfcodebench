#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
    uint64_t* hist = (uint64_t*)malloc(buckets * sizeof(uint64_t));
    if (!hist) return 0;

    uint64_t hash = 0;
    size_t n4 = n & ~(size_t)3;

    for (int iter = 0; iter < iters; ++iter) {
        memset(hist, 0, buckets * sizeof(uint64_t));

        for (size_t i = 0; i < n4; i += 4) {
            hist[keys[i]]   += vals[i];
            hist[keys[i+1]] += vals[i+1];
            hist[keys[i+2]] += vals[i+2];
            hist[keys[i+3]] += vals[i+3];
        }
        for (size_t i = n4; i < n; ++i) {
            hist[keys[i]] += vals[i];
        }

        hash = 1469598103934665603ULL;
        for (uint32_t i = 0; i < buckets; ++i) {
            hash ^= (hist[i] + 1ULL);
            hash *= 1099511628211ULL;
        }
    }

    free(hist);
    return hash;
}