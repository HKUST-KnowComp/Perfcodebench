#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
    /* one reusable zeroed buffer for the whole call */
    uint64_t* hist = (uint64_t*)calloc(buckets, sizeof(uint64_t));
    if (!hist) return 0;

    const uint64_t prime = 1099511628211ULL;
    uint64_t hash = 1469598103934665603ULL;

    for (int iter = 0; iter < iters; ++iter) {
        /* clear histogram quickly – memset is vectorised */
        memset(hist, 0, buckets * sizeof(uint64_t));

        /* unroll summation 8× to increase ILP */
        size_t i = 0;
        size_t top = n & ~(size_t)7;
        for (; i < top; i += 8) {
            hist[keys[i  ]] += vals[i  ];
            hist[keys[i+1]] += vals[i+1];
            hist[keys[i+2]] += vals[i+2];
            hist[keys[i+3]] += vals[i+3];
            hist[keys[i+4]] += vals[i+4];
            hist[keys[i+5]] += vals[i+5];
            hist[keys[i+6]] += vals[i+6];
            hist[keys[i+7]] += vals[i+7];
        }
        /* tail */
        for (; i < n; ++i) hist[keys[i]] += vals[i];

        /* FNV-1a over buckets */
        for (uint32_t b = 0; b < buckets; ++b) {
            hash ^= (hist[b] + 1ULL);
            hash *= prime;
        }
    }

    free(hist);
    return hash;
}