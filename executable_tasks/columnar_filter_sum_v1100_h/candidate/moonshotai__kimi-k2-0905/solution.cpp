#include "interface.h"
#include <cstdint>
#include <vector>
#include <immintrin.h>

uint64_t filter_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint8_t>& flags,
    uint32_t key_threshold,
    int iters)
{
    const std::size_t n = keys.size();
    const uint32_t*  k = keys.data();
    const uint32_t*  v = values.data();
    const uint8_t*   f = flags.data();

    uint64_t total = 0;

    for (int iter = 0; iter < iters; ++iter) {
        __m256i vacc = _mm256_setzero_si256();
        std::size_t i = 0;

        // vectorised main loop: 8 elements per iteration
        const std::size_t vec_end = n & ~std::size_t(7);
        for (; i < vec_end; i += 8) {
            __m256i kv = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(k + i));
            __m256i vv = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(v + i));

            // flags as 32-bit mask
            __m256i fv = _mm256_set_epi32(
                f[i+7], f[i+6], f[i+5], f[i+4],
                f[i+3], f[i+2], f[i+1], f[i+0]);
            __m256i maskF = _mm256_cmpgt_epi32(fv, _mm256_setzero_si256());

            // key < key_threshold
            __m256i limit = _mm256_set1_epi32(static_cast<int32_t>(key_threshold));
            __m256i maskT = _mm256_cmpgt_epi32(limit, kv);

            // (key & 3) == 0
            __m256i and3 = _mm256_and_si256(kv, _mm256_set1_epi32(3));
            __m256i maskA = _mm256_cmpeq_epi32(and3, _mm256_setzero_si256());

            // combine all conditions
            __m256i mask = _mm256_and_si256(_mm256_and_si256(maskF, maskT), maskA);

            // zero values that fail the filter
            vv = _mm256_and_si256(vv, mask);
            vacc = _mm256_add_epi64(
                vacc,
                _mm256_unpacklo_epi32(vv, _mm256_setzero_si256()));
            vacc = _mm256_add_epi64(
                vacc,
                _mm256_unpackhi_epi32(vv, _mm256_setzero_si256()));
        }

        // horizontal sum of vector accumulator
        __m256i shuf = _mm256_shuffle_epi32(vacc, 0x4E);
        __m256i sums = _mm256_add_epi64(vacc, shuf);
        uint64_t partial = static_cast<uint64_t>(_mm256_extract_epi64(sums, 0)) +
                           static_cast<uint64_t>(_mm256_extract_epi64(sums, 1));

        // scalar tail
        for (; i < n; ++i) {
            if (f[i] && k[i] < key_threshold && ((k[i] & 3u) == 0u))
                partial += v[i];
        }

        total = partial;
    }
    return total;
}