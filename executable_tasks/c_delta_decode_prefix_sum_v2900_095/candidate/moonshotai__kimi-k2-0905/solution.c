#include "interface.h"

#include <stdint.h>
#include <immintrin.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
    const int64_t magic = 0x9E3779B1LL;
    uint64_t grand_total = 0;

    for (int iter = 0; iter < iters; ++iter) {
        int64_t  prefix = 0;
        uint64_t local_total = 0;
        size_t   i = 0;

        const size_t vec = count & ~(size_t)7;
        __m256i acc = _mm256_setzero_si256();

        for (; i < vec; i += 8) {
            __m256i d = _mm256_loadu_si256((__m256i*)&deltas[i]);
            __m256i lo = _mm256_cvtepi32_epi64(_mm256_castsi256_si128(d));
            __m256i hi = _mm256_cvtepi32_epi64(_mm256_extracti128_si256(d,1));

            acc = _mm256_add_epi64(acc, lo);
            __m256i t = _mm256_add_epi64(acc, _mm256_set1_epi64(magic));
            local_total += (uint64_t)_mm256_extract_epi64(t,0) +
                           (uint64_t)_mm256_extract_epi64(t,1) +
                           (uint64_t)_mm256_extract_epi64(t,2) +
                           (uint64_t)_mm256_extract_epi64(t,3);

            acc = _mm256_add_epi64(acc, hi);
            t = _mm256_add_epi64(acc, _mm256_set1_epi64(magic));
            local_total += (uint64_t)_mm256_extract_epi64(t,0) +
                           (uint64_t)_mm256_extract_epi64(t,1) +
                           (uint64_t)_mm256_extract_epi64(t,2) +
                           (uint64_t)_mm256_extract_epi64(t,3);
        }

        for (; i < count; ++i) {
            prefix += deltas[i];
            local_total += (uint64_t)(prefix + magic);
        }

        grand_total = local_total;
    }

    return grand_total;
}