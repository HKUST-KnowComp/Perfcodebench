#include "interface.h"

#include <cstdint>
#include <vector>
#include <immintrin.h>

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters)
{
    const std::size_t n = keys.size();
    const uint32_t*  k = keys.data();
    const uint8_t*   f = flags.data();
    const uint32_t*  p = payloads.data();

    for (int iter = 0; iter < iters; ++iter) {
        __m256i vacc = _mm256_setzero_si256();
        __m256i vthreshold = _mm256_set1_epi32(static_cast<int32_t>(threshold));
        __m256i vand7 = _mm256_set1_epi32(7);

        std::size_t i = 0;
        const std::size_t vec_end = n & ~7;

        for (; i < vec_end; i += 8) {
            __m256i vk   = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(k + i));
            __m256i vf8  = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(f + i));

            __m256i vf32 = _mm256_cvtepu8_epi32(_mm_loadl_epi64(reinterpret_cast<const __m128i*>(f + i)));
            __m256i mask_flag = _mm256_cmpgt_epi32(vf32, _mm256_setzero_si256());

            __m256i mask_lt   = _mm256_cmpgt_epi32(vthreshold, vk);
            __m256i mask_and7 = _mm256_cmpeq_epi32(_mm256_and_si256(vk, vand7), _mm256_setzero_si256());

            __m256i mask = _mm256_and_si256(_mm256_and_si256(mask_flag, mask_lt), mask_and7);

            __m256i vp   = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(p + i));
            __m256i vp_masked = _mm256_and_si256(vp, mask);
            vacc = _mm256_add_epi64(vacc, _mm256_unpacklo_epi32(vp_masked, _mm256_setzero_si256()));
            vacc = _mm256_add_epi64(vacc, _mm256_unpackhi_epi32(vp_masked, _mm256_setzero_si256()));
        }

        uint64_t scalar_sum = 0;
        for (; i < n; ++i) {
            if (f[i] && k[i] < threshold && ((k[i] & 7u) == 0u)) {
                scalar_sum += static_cast<uint64_t>(p[i]);
            }
        }

        __m256i sum_lo = _mm256_unpacklo_epi64(vacc, _mm256_setzero_si256());
        __m256i sum_hi = _mm256_unpackhi_epi64(vacc, _mm256_setzero_si256());
        __m256i sum128 = _mm256_add_epi64(sum_lo, sum_hi);
        sum128 = _mm256_add_epi64(sum128, _mm256_srli_si256(sum128, 8));
        scalar_sum += static_cast<uint64_t>(_mm256_extract_epi64(sum128, 0));

        if (iter == iters - 1) return scalar_sum;
    }
    return 0;
}