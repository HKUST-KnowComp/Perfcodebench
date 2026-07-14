#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
    /* Allocate once, reuse every iteration */
    uint64_t* hist = (uint64_t*)calloc(buckets, sizeof(uint64_t));
    if (!hist) return 0;

    const uint64_t kMul = 1099511628211ULL;
    const uint64_t kInit = 1469598103934665603ULL;

    for (int iter = 0; iter < iters; ++iter) {
        /* Zero histogram quickly (memset is vectorised) */
        memset(hist, 0, buckets * sizeof(uint64_t));

        /* Aggressively unrolled summation loop */
        size_t i = 0;
        size_t limit = n & ~(size_t)3;
        for (; i < limit; i += 4) {
            hist[keys[i  ]] += vals[i  ];
            hist[keys[i+1]] += vals[i+1];
            hist[keys[i+2]] += vals[i+2];
            hist[keys[i+3]] += vals[i+3];
        }
        for (; i < n; ++i) {
            hist[keys[i]] += vals[i];
        }

        /* Final checksum pass */
        uint64_t hash = kInit;
        for (uint32_t b = 0; b < buckets; ++b) {
            hash ^= (hist[b] + 1ULL);
            hash *= kMul;
        }
        if (iter == iters - 1) {
            free(hist);
            return hash;
        }
    }
    /* Never reached */
    free(hist);
    return 0;
}