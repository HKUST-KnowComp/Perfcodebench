#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
    if (iters <= 0) return 0;

    uint64_t* hist = (uint64_t*)calloc((size_t)buckets, sizeof(uint64_t));
    if (!hist) return 0;

    uint64_t hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        const uint32_t* k = keys;
        const uint32_t* v = vals;
        size_t i = 0;

        size_t n4 = n & ~(size_t)3;
        for (; i < n4; i += 4) {
            uint32_t k0 = k[i + 0];
            uint32_t k1 = k[i + 1];
            uint32_t k2 = k[i + 2];
            uint32_t k3 = k[i + 3];
            hist[k0] += v[i + 0];
            hist[k1] += v[i + 1];
            hist[k2] += v[i + 2];
            hist[k3] += v[i + 3];
        }
        for (; i < n; ++i) {
            hist[k[i]] += v[i];
        }

        uint64_t h = 1469598103934665603ULL;
        uint32_t j = 0;
        uint32_t b4 = buckets & ~((uint32_t)3);
        for (; j < b4; j += 4) {
            h ^= (hist[j + 0] + 1ULL);
            h *= 1099511628211ULL;
            h ^= (hist[j + 1] + 1ULL);
            h *= 1099511628211ULL;
            h ^= (hist[j + 2] + 1ULL);
            h *= 1099511628211ULL;
            h ^= (hist[j + 3] + 1ULL);
            h *= 1099511628211ULL;
        }
        for (; j < buckets; ++j) {
            h ^= (hist[j] + 1ULL);
            h *= 1099511628211ULL;
        }
        hash = h;

        if (iter + 1 < iters) {
            memset(hist, 0, (size_t)buckets * sizeof(uint64_t));
        }
    }

    free(hist);
    return hash;
}
