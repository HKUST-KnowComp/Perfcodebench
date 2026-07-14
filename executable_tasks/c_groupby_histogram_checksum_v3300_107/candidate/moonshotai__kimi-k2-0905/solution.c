#include <stdint.h>
#include <string.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
    uint64_t hash = 0;
    uint64_t* hist = (uint64_t*)aligned_alloc(64, (size_t)buckets * sizeof(uint64_t));
    if (!hist) return 0;

    const uint32_t* restrict k = keys;
    const uint32_t* restrict v = vals;

    for (int iter = 0; iter < iters; ++iter) {
        memset(hist, 0, (size_t)buckets * sizeof(uint64_t));

        size_t i = 0;
        size_t n8 = n & ~(size_t)7;
        for (; i < n8; i += 8) {
            hist[k[i  ]] += v[i  ];
            hist[k[i+1]] += v[i+1];
            hist[k[i+2]] += v[i+2];
            hist[k[i+3]] += v[i+3];
            hist[k[i+4]] += v[i+4];
            hist[k[i+5]] += v[i+5];
            hist[k[i+6]] += v[i+6];
            hist[k[i+7]] += v[i+7];
        }
        for (; i < n; ++i) hist[k[i]] += v[i];

        uint64_t h0 = 1469598103934665603ULL;
        uint32_t b4 = buckets & ~(uint32_t)3;
        uint32_t j = 0;
        for (; j < b4; j += 4) {
            h0 ^= (hist[j  ] + 1ULL);
            h0 *= 1099511628211ULL;
            h0 ^= (hist[j+1] + 1ULL);
            h0 *= 1099511628211ULL;
            h0 ^= (hist[j+2] + 1ULL);
            h0 *= 1099511628211ULL;
            h0 ^= (hist[j+3] + 1ULL);
            h0 *= 1099511628211ULL;
        }
        for (; j < buckets; ++j) {
            h0 ^= (hist[j] + 1ULL);
            h0 *= 1099511628211ULL;
        }
        hash = h0;
    }
    free(hist);
    return hash;
}