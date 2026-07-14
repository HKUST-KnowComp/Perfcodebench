#include <stdint.h>
#include <string.h>
#include <immintrin.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
    static thread_local uint64_t* hist = NULL;
    static thread_local uint32_t prev_buckets = 0;
    if (!hist || buckets != prev_buckets) {
        free(hist);
        hist = (uint64_t*)aligned_alloc(32, (size_t)buckets * sizeof(uint64_t));
        prev_buckets = buckets;
    }

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        memset(hist, 0, buckets * sizeof(uint64_t));

        size_t i = 0;
        const __m256i zero = _mm256_setzero_si256();
        for (; i + 8 <= n; i += 8) {
            __m256i k = _mm256_loadu_si256((const __m256i*)(keys + i));
            __m256i v = _mm256_loadu_si256((const __m256i*)(vals + i));
            for (int j = 0; j < 8; ++j) {
                uint32_t kj = ((uint32_t*)&k)[j];
                uint32_t vj = ((uint32_t*)&v)[j];
                hist[kj] += vj;
            }
        }
        for (; i < n; ++i) hist[keys[i]] += vals[i];

        hash = 1469598103934665603ULL;
        for (uint32_t b = 0; b < buckets; ++b) {
            hash ^= (hist[b] + 1ULL);
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}