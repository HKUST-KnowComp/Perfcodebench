#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
    if (iters <= 0) return 0;

    uint64_t* hist = (uint64_t*)calloc((size_t)buckets, sizeof(uint64_t));
    if (!hist) return 0;

    uint64_t hash = 0;
    const uint64_t fnv_offset = 1469598103934665603ULL;
    const uint64_t fnv_prime = 1099511628211ULL;

    for (int iter = 0; iter < iters; ++iter) {
        const uint32_t* k = keys;
        const uint32_t* v = vals;
        size_t i = 0;

        for (; i + 7 < n; i += 8) {
            hist[k[0]] += v[0];
            hist[k[1]] += v[1];
            hist[k[2]] += v[2];
            hist[k[3]] += v[3];
            hist[k[4]] += v[4];
            hist[k[5]] += v[5];
            hist[k[6]] += v[6];
            hist[k[7]] += v[7];
            k += 8;
            v += 8;
        }
        for (; i < n; ++i) {
            hist[*k++] += *v++;
        }

        hash = fnv_offset;
        for (uint32_t b = 0; b < buckets; ++b) {
            hash ^= (hist[b] + 1ULL);
            hash *= fnv_prime;
        }

        if (iter + 1 < iters) {
            memset(hist, 0, (size_t)buckets * sizeof(uint64_t));
        }
    }

    free(hist);
    return hash;
}
