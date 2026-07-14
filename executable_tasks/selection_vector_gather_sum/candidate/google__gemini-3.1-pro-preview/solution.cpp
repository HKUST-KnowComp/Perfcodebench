#include "interface.h"
#include <immintrin.h>
#include <cstdint>
#include <vector>

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters) {
    
    const size_t n = keys.size();
    if (n == 0) return 0;

    uint64_t final_sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        size_t i = 0;

        // SIMD processing: 8 elements at a time using AVX2
        __m256i v_sum_lo = _mm256_setzero_si256();
        __m256i v_sum_hi = _mm256_setzero_si256();
        __m256i v_threshold = _mm256_set1_epi32(threshold);
        __m256i v_seven = _mm256_set1_epi32(7);
        __m256i v_zero = _mm256_setzero_si256();

        for (; i + 8 <= n; i += 8) {
            // Load keys and payloads
            __m256i v_keys = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&keys[i]));
            __m256i v_pay = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&payloads[i]));

            // Load 8 flags and extend to 32-bit integers
            __m128i v_flags_8 = _mm_loadl_epi64(reinterpret_cast<const __m128i*>(&flags[i]));
            __m256i v_flags = _mm256_cvtepu8_epi32(v_flags_8);

            // Predicate 1: keys[i] < threshold
            // _mm256_cmpgt_epi32 returns true if a > b. We want keys < threshold.
            __m256i m_lt = _mm256_cmpgt_epi32(v_threshold, v_keys);

            // Predicate 2: (keys[i] & 7) == 0
            __m256i v_and7 = _mm256_and_si256(v_keys, v_seven);
            __m256i m_mod = _mm256_cmpeq_epi32(v_and7, v_zero);

            // Predicate 3: flags[i] != 0
            __m256i m_flag = _mm256_cmpgt_epi32(v_flags, v_zero);

            // Combine predicates
            __m256i m_final = _mm256_and_si256(_mm256_and_si256(m_lt, m_mod), m_flag);

            // Mask payloads: elements not meeting criteria become 0
            __m256i v_masked_pay = _mm256_and_si256(m_final, v_pay);

            // Unpack 32-bit payloads to 64-bit to accumulate without overflow
            __m256i v_pay_lo = _mm256_unpacklo_epi32(v_masked_pay, v_zero);
            __m256i v_pay_hi = _mm256_unpackhi_epi32(v_masked_pay, v_zero);

            v_sum_lo = _mm256_add_epi64(v_sum_lo, v_pay_lo);
            v_sum_hi = _mm256_add_epi64(v_sum_hi, v_pay_hi);
        }

        // Horizontal sum of the SIMD registers
        __m256i v_sum_total = _mm256_add_epi64(v_sum_lo, v_sum_hi);
        alignas(32) uint64_t temp_sums[4];
        _mm256_store_si256(reinterpret_cast<__m256i*>(temp_sums), v_sum_total);
        current_sum = temp_sums[0] + temp_sums[1] + temp_sums[2] + temp_sums[3];

        // Scalar tail processing
        for (; i < n; ++i) {
            if (flags[i] && keys[i] < threshold && ((keys[i] & 7u) == 0u)) {
                current_sum += static_cast<uint64_t>(payloads[i]);
            }
        }
        final_sum = current_sum;
    }

    return final_sum;
}