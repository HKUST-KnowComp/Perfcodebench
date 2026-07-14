#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
    uint64_t hash = 0;
    if (iters <= 0) return 0;

    uint64_t* hist = (uint64_t*)calloc((size_t)buckets, sizeof(uint64_t));
    if (!hist) return 0;

    if (buckets == 0) {
        free(hist);
        return 1469598103934665603ULL;
    }

    for (int iter = 0; iter < iters; ++iter) {
        memset(hist, 0, (size_t)buckets * sizeof(uint64_t));

        size_t i = 0;
        size_t n4 = n & ~(size_t)3;
        for (; i < n4; i += 4) {
            uint32_t k0 = keys[i];
            uint32_t k1 = keys[i + 1];
            uint32_t k2 = keys[i + 2];
            uint32_t k3 = keys[i + 3];
            hist[k0] += vals[i];
            hist[k1] += vals[i + 1];
            hist[k2] += vals[i + 2];
            hist[k3] += vals[i + 3];
        }
        for (; i < n; ++i) {
            hist[keys[i]] += vals[i];
        }

        uint64_t h = 1469598103934665603ULL;
        uint32_t j = 0;
        uint32_t b4 = buckets & ~((uint32_t)3);
        for (; j < b4; j += 4) {
            h ^= (hist[j] + 1ULL);
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
    }

    free(hist);
    return hash;
}
