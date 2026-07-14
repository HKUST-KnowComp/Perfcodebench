#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
    if (iters <= 0) return 0;

    uint64_t* hist = (uint64_t*)calloc((size_t)buckets, sizeof(uint64_t));
    if (!hist) return 0;

    uint64_t hash = 0;
    const uint64_t FNV_OFFSET = 1469598103934665603ULL;
    const uint64_t FNV_PRIME = 1099511628211ULL;
    const size_t bsz = (size_t)buckets * sizeof(uint64_t);

    for (int iter = 0; iter < iters; ++iter) {
        memset(hist, 0, bsz);

        size_t i = 0;
        size_t n4 = n & ~(size_t)3;
        for (; i < n4; i += 4) {
            hist[keys[i]] += vals[i];
            hist[keys[i + 1]] += vals[i + 1];
            hist[keys[i + 2]] += vals[i + 2];
            hist[keys[i + 3]] += vals[i + 3];
        }
        for (; i < n; ++i) {
            hist[keys[i]] += vals[i];
        }

        uint64_t h = FNV_OFFSET;
        uint32_t j = 0;
        uint32_t b4 = buckets & ~((uint32_t)3);
        for (; j < b4; j += 4) {
            h ^= (hist[j] + 1ULL);
            h *= FNV_PRIME;
            h ^= (hist[j + 1] + 1ULL);
            h *= FNV_PRIME;
            h ^= (hist[j + 2] + 1ULL);
            h *= FNV_PRIME;
            h ^= (hist[j + 3] + 1ULL);
            h *= FNV_PRIME;
        }
        for (; j < buckets; ++j) {
            h ^= (hist[j] + 1ULL);
            h *= FNV_PRIME;
        }
        hash = h;
    }

    free(hist);
    return hash;
}
