#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
    uint64_t* hist = (uint64_t*)malloc(buckets * sizeof(uint64_t));
    if (!hist) return 0;

    uint64_t hash = 0;
    size_t bucket_bytes = buckets * sizeof(uint64_t);

    for (int iter = 0; iter < iters; ++iter) {
        memset(hist, 0, bucket_bytes);

        size_t i = 0;
        uint64_t* restrict h = hist;

        for (; i + 7 < n; i += 8) {
            h[keys[i]]   += vals[i];
            h[keys[i+1]] += vals[i+1];
            h[keys[i+2]] += vals[i+2];
            h[keys[i+3]] += vals[i+3];
            h[keys[i+4]] += vals[i+4];
            h[keys[i+5]] += vals[i+5];
            h[keys[i+6]] += vals[i+6];
            h[keys[i+7]] += vals[i+7];
        }
        for (; i < n; ++i) {
            h[keys[i]] += vals[i];
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