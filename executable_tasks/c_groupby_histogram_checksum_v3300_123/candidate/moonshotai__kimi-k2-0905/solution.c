#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
    /* allocate once, reuse every iteration */
    uint64_t* hist = (uint64_t*)malloc(sizeof(uint64_t) * buckets);
    if (!hist) return 0;

    /* loop-invariant constants to help the compiler */
    const uint32_t* k = keys;
    const uint32_t* v = vals;
    const size_t len = n;

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        /* fast clear: memset is vectorised on all modern libc */
        __builtin_memset(hist, 0, sizeof(uint64_t) * buckets);

        /* unroll 4× to reduce loop-control overhead and improve ILP */
        size_t i = 0;
        for (; i + 3 < len; i += 4) {
            hist[k[i  ]] += v[i  ];
            hist[k[i+1]] += v[i+1];
            hist[k[i+2]] += v[i+2];
            hist[k[i+3]] += v[i+3];
        }
        /* tail */
        for (; i < len; ++i) {
            hist[k[i]] += v[i];
        }

        /* FNV-1a checksum over the histogram */
        uint64_t h = 1469598103934665603ULL;
        for (uint32_t idx = 0; idx < buckets; ++idx) {
            h ^= (hist[idx] + 1ULL);
            h *= 1099511628211ULL;
        }
        hash = h;           /* value used in next iteration if iters>1 */
    }
    free(hist);
    return hash;
}