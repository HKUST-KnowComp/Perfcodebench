#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#if defined(__AVX2__)
#include <immintrin.h>
#endif

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
    /* allocate once, reuse every iteration */
    uint64_t* hist64 = (uint64_t*)calloc(buckets ? buckets : 1, sizeof(uint64_t));
    if (!hist64) return 0;

    /* temporary 32-bit buffer for faster vectorized adds */
    uint32_t* hist32 = (uint32_t*)hist64;

    /* number of 32-bit buckets we actually touch */
    const size_t buckets32 = (size_t)buckets * 2;

    /* --- main loop --- */
    for (int iter = 0; iter < iters; ++iter) {
        /* fast zero: memset is vectorized by the libc */
        memset(hist32, 0, buckets * sizeof(uint64_t));

        size_t i = 0;

#if defined(__AVX2__)
        /* process 8 elements per iteration with AVX2 */
        const size_t n_vec = n & ~7ULL;
        for (; i < n_vec; i += 8) {
            __m256i k = _mm256_loadu_si256((const __m256i*)(keys + i));
            __m256i v = _mm256_loadu_si256((const __m256i*)(vals + i));

            /* scatter-add: do 8 scalar adds; scatter intrinsics are not worth it here */
            for (int lane = 0; lane < 8; ++lane) {
                uint32_t idx = ((uint32_t*)&k)[lane];
                ((uint32_t*)&hist32)[idx] += ((uint32_t*)&v)[lane];
            }
        }
#endif

        /* scalar tail */
        for (; i < n; ++i) {
            hist32[keys[i]] += vals[i];
        }

        /* --- checksum --- */
        uint64_t hash = 1469598103934665603ULL;
        for (uint32_t b = 0; b < buckets; ++b) {
            hash ^= (hist64[b] + 1ULL);
            hash *= 1099511628211ULL;
        }

        /* store hash so that the compiler doesn't optimize the loop away */
        hist64[0] = hash;
    }

    uint64_t final_hash = hist64[0];
    free(hist64);
    return final_hash;
}