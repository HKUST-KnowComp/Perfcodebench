#include "interface.h"

#include <cstdint>
#include <cstddef>

#if defined(__AVX2__)
#include <immintrin.h>
#endif

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters) {

    const std::size_t n = keys.size();
    const uint32_t* __restrict k_ptr = keys.data();
    const uint8_t*  __restrict f_ptr = flags.data();
    const uint32_t* __restrict p_ptr = payloads.data();

    uint64_t sum = 0;

#if defined(__AVX2__)
    const __m256i v_threshold = _mm256_set1_epi32(static_cast<int>(threshold));
    const __m256i v_seven = _mm256_set1_epi32(7);
    const __m256i v_zero = _mm256_setzero_si256();

    for (int iter = 0; iter < iters; ++iter) {
        __m256i v_sum_lo = _mm256_setzero_si256();
        __m256i v_sum_hi = _mm256_setzero_si256();

        std::size_t i = 0;
        const std::size_t vec_end = n & ~std::size_t(7);

        for (; i < vec_end; i += 8) {
            // Load 8 keys
            __m256i v_keys = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(k_ptr + i));

            // Load 8 flags (bytes) and expand to 32-bit
            __m128i v_flags_8 = _mm_loadl_epi64(reinterpret_cast<const __m128i*>(f_ptr + i));
            __m256i v_flags = _mm256_cvtepu8_epi32(v_flags_8);

            // flags != 0
            __m256i cmp_flags = _mm256_cmpgt_epi32(v_flags, v_zero);

            // keys < threshold (unsigned compare via signed trick: subtract and check sign)
            // For unsigned compare a < b: use _mm256_cmpgt_epi32(b, a) when both < 2^31
            // But threshold could be large. Use: a < b <=> (a ^ 0x80000000) < (b ^ 0x80000000) signed
            __m256i bias = _mm256_set1_epi32(static_cast<int>(0x80000000u));
            __m256i v_keys_biased = _mm256_xor_si256(v_keys, bias);
            __m256i v_thresh_biased = _mm256_xor_si256(v_threshold, bias);
            __m256i cmp_thresh = _mm256_cmpgt_epi32(v_thresh_biased, v_keys_biased);

            // (keys & 7) == 0
            __m256i v_and7 = _mm256_and_si256(v_keys, v_seven);
            __m256i cmp_and7 = _mm256_cmpeq_epi32(v_and7, v_zero);

            // Combined mask
            __m256i mask = _mm256_and_si256(cmp_flags, _mm256_and_si256(cmp_thresh, cmp_and7));

            // Load payloads
            __m256i v_payloads = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(p_ptr + i));

            // Mask payloads (zero out non-selected)
            __m256i v_selected = _mm256_and_si256(v_payloads, mask);

            // Accumulate as 64-bit: split into low and high 4 elements
            __m256i lo = _mm256_cvtepu32_epi64(_mm256_castsi256_si128(v_selected));
            __m256i hi = _mm256_cvtepu32_epi64(_mm256_extracti128_si256(v_selected, 1));

            v_sum_lo = _mm256_add_epi64(v_sum_lo, lo);
            v_sum_hi = _mm256_add_epi64(v_sum_hi, hi);
        }

        // Horizontal sum of v_sum_lo and v_sum_hi
        __m256i v_total = _mm256_add_epi64(v_sum_lo, v_sum_hi);
        __m128i lo128 = _mm256_castsi256_si128(v_total);
        __m128i hi128 = _mm256_extracti128_si256(v_total, 1);
        __m128i sum128 = _mm_add_epi64(lo128, hi128);
        sum = static_cast<uint64_t>(_mm_extract_epi64(sum128, 0)) +
              static_cast<uint64_t>(_mm_extract_epi64(sum128, 1));

        // Scalar tail
        for (; i < n; ++i) {
            uint32_t key = k_ptr[i];
            if (f_ptr[i] && key < threshold && ((key & 7u) == 0u)) {
                sum += static_cast<uint64_t>(p_ptr[i]);
            }
        }
    }
#else
    // Scalar fallback
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        for (std::size_t i = 0; i < n; ++i) {
            uint32_t key = k_ptr[i];
            if (f_ptr[i] && key < threshold && ((key & 7u) == 0u)) {
                sum += static_cast<uint64_t>(p_ptr[i]);
            }
        }
    }
#endif

    return sum;
}
