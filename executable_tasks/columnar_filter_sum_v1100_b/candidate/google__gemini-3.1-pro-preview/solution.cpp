#include "interface.h"
#include <immintrin.h>
#include <cstdint>
#include <vector>

uint64_t filter_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint8_t>& flags,
    uint32_t key_threshold,
    int iters) {
    
    const size_t n = keys.size();
    uint64_t total_sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        size_t i = 0;

        // SIMD constants
        __m256i v_threshold = _mm256_set1_epi32(static_cast<int32_t>(key_threshold ^ 0x80000000));
        __m256i v_three = _mm256_set1_epi32(3);
        __m256i v_zero = _mm256_setzero_si256();
        __m256i v_bias = _mm256_set1_epi32(0x80000000);
        __m256i acc_sum = _mm256_setzero_si256();

        // Process 8 elements at a time
        for (; i + 7 < n; i += 8) {
            // Load flags (8 bytes) and expand to 8x32-bit
            __m128i f_small = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&flags[i]));
            __m256i f = _mm256_cvtepu8_epi32(f_small);
            __m256i mask_f = _mm256_cmpgt_epi32(f, v_zero);

            // Load keys and check (keys < threshold) and ((keys & 3) == 0)
            __m256i k = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&keys[i]));
            
            // Unsigned comparison: (k ^ 0x80000000) < (threshold ^ 0x80000000)
            __m256i k_biased = _mm256_xor_si256(k, v_bias);
            __m256i mask_k_lt = _mm256_cmpgt_epi32(v_threshold, k_biased);

            __m256i k_and_3 = _mm256_and_si256(k, v_three);
            __m256i mask_k_mod = _mm256_cmpeq_epi32(k_and_3, v_zero);

            // Combine masks
            __m256i final_mask = _mm256_and_si256(mask_f, _mm256_and_si256(mask_k_lt, mask_k_mod));

            // Load values and apply mask
            __m256i v = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&values[i]));
            __m256i filtered_v = _mm256_and_si256(v, final_mask);

            // Accumulate into 64-bit sums to prevent overflow
            __m256i v_lo = _mm256_unpacklo_epi32(filtered_v, v_zero);
            __m256i v_hi = _mm256_unpackhi_epi32(filtered_v, v_zero);
            acc_sum = _mm256_add_epi64(acc_sum, _mm256_add_epi64(v_lo, v_hi));
        }

        // Horizontal sum of the SIMD accumulator
        uint64_t temp_acc[4];
        _mm256_storeu_si256(reinterpret_cast<__m256i*>(temp_acc), acc_sum);
        current_sum = temp_acc[0] + temp_acc[1] + temp_acc[2] + temp_acc[3];

        // Scalar tail
        for (; i < n; ++i) {
            if (flags[i] && keys[i] < key_threshold && ((keys[i] & 3u) == 0u)) {
                current_sum += static_cast<uint64_t>(values[i]);
            }
        }
        total_sum = current_sum;
    }

    return total_sum;
}