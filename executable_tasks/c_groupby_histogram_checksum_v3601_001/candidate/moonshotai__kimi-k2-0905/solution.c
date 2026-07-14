#include <stdint.h>
#include <stdlib.h>
#include <immintrin.h>
#include <string.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
    uint64_t hash = 0;
    /* allocate once, aligned for AVX2 */
    uint64_t* hist = (uint64_t*)_mm_malloc(buckets * sizeof(uint64_t), 32);
    if (!hist) return 0;

    for (int iter = 0; iter < iters; ++iter) {
        /* fast clear */
        memset(hist, 0, buckets * sizeof(uint64_t));

        /* hot loop vectorised: eight buckets per iteration */
        size_t i = 0;
        if (buckets >= 8) {
            for (; i + 7 < n; i += 8) {
                __m256i k = _mm256_loadu_si256((__m256i*)&keys[i]);
                __m256i v = _mm256_loadu_si256((__m256i*)&vals[i]);

                /* scatter-add would need AVX-512; instead we unroll 8 scalar adds,
                   but ensure the compiler keeps keys in a register */
                #pragma unroll
                for (int j = 0; j < 8; ++j) {
                    uint32_t idx = ((uint32_t*)&k)[j];
                    hist[idx] += ((uint32_t*)&v)[j];
                }
            }
        }
        /* scalar tail */
        for (; i < n; ++i) hist[keys[i]] += vals[i];

        /* FNV-1a over buckets */
        hash = 1469598103934665603ULL;
        for (uint32_t b = 0; b < buckets; ++b) {
            hash ^= (hist[b] + 1ULL);
            hash *= 1099511628211ULL;
        }
    }
    _mm_free(hist);
    return hash;
}