#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
    uint64_t* hist = (uint64_t*)malloc(buckets * sizeof(uint64_t));
    if (!hist) return 0;

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        memset(hist, 0, buckets * sizeof(uint64_t));

        size_t i = 0;
        size_t n4 = n & ~(size_t)3;
        for (; i < n4; i += 4) {
            hist[keys[i  ]] += vals[i  ];
            hist[keys[i+1]] += vals[i+1];
            hist[keys[i+2]] += vals[i+2];
            hist[keys[i+3]] += vals[i+3];
        }
        for (; i < n; ++i) hist[keys[i]] += vals[i];

        uint64_t h = 1469598103934665603ULL;
        uint32_t b = 0;
        uint32_t b4 = buckets & ~(uint32_t)3;
        for (; b < b4; b += 4) {
            h ^= (hist[b  ] + 1ULL); h *= 1099511628211ULL;
            h ^= (hist[b+1] + 1ULL); h *= 1099511628211ULL;
            h ^= (hist[b+2] + 1ULL); h *= 1099511628211ULL;
            h ^= (hist[b+3] + 1ULL); h *= 1099511628211ULL;
        }
        for (; b < buckets; ++b) {
            h ^= (hist[b] + 1ULL);
            h *= 1099511628211ULL;
        }
        hash = h;
    }
    free(hist);
    return hash;
}