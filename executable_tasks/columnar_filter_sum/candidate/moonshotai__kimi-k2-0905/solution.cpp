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
        uint64_t sum = 0;
        std::size_t i = 0;

        // SIMD main loop: 4 elements per iteration
        const std::size_t simd_end = n & ~std::size_t(3);
        for (; i < simd_end; i += 4) {
            __m128i key4   = _mm_loadu_si128((__m128i*)(k + i));
            __m128i flag4  = _mm_cvtepu8_epi32(_mm_loadu_si32(f + i));
            __m128i val4   = _mm_loadu_si128((__m128i*)(v + i));

            // flag != 0  -> 0xFFFFFFFF per lane
            __m128i mask_flag = _mm_cmpgt_epi32(flag4, _mm_setzero_si128());
            // key < threshold
            __m128i mask_thresh = _mm_cmplt_epu32(key4, _mm_set1_epi32(key_threshold));
            // (key & 3) == 0
            __m128i mask_align = _mm_cmpeq_epi32(_mm_and_si128(key4, _mm_set1_epi32(3)), _mm_setzero_si128());

            // combine masks
            __m128i mask = _mm_and_si128(_mm_and_si128(mask_flag, mask_thresh), mask_align);

            // zero out non-qualifying values
            __m128i masked_val = _mm_and_si128(val4, mask);

            // horizontal add of 4 uint32 into scalar
            __m128i sum64 = _mm_add_epi64(_mm_unpacklo_epi32(masked_val, _mm_setzero_si128()),
                                          _mm_unpacklo_epi32(_mm_srli_si128(masked_val, 8), _mm_setzero_si128()));
            uint64_t lo = static_cast<uint64_t>(_mm_cvtsi128_si64(sum64));
            uint64_t hi = static_cast<uint64_t>(_mm_cvtsi128_si64(_mm_srli_si128(sum64, 8)));
            sum += lo + hi;
        }

        // scalar tail
        for (; i < n; ++i) {
            if (f[i] && k[i] < key_threshold && ((k[i] & 3u) == 0u)) {
                sum += v[i];
            }
        }
        total = sum;
    }
    return total;
}