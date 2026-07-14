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
    const uint32_t* k_ptr = keys.data();
    const uint8_t* f_ptr = flags.data();
    const uint32_t* p_ptr = payloads.data();

    uint64_t final_sum = 0;

    // Pre-broadcast constants for SIMD
    const __m256i v_threshold = _mm256_set1_epi32(static_cast<int32_t>(threshold));
    const __m256i v_seven = _mm256_set1_epi32(7);
    const __m256i v_zero = _mm256_setzero_si256();
    const __m256i v_sign_bit = _mm256_set1_epi32(0x80000000);
    const __m256i v_threshold_shifted = _mm256_xor_si256(v_threshold, v_sign_bit);

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_iter_sum = 0;
        size_t i = 0;

        // Process in blocks of 8 using AVX2
        for (; i + 7 < n; i += 8) {
            __m256i k = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(k_ptr + i));
            __m128i f_small = _mm_loadl_epi64(reinterpret_cast<const __m128i*>(f_ptr + i));
            __m256i f = _mm256_cvtepu8_epi32(f_small);
            __m256i p = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(p_ptr + i));

            // Condition 1: flags[i] != 0
            __m256i mask_f = _mm256_cmpgt_epi32(f, v_zero);

            // Condition 2: keys[i] < threshold (Unsigned comparison trick)
            __m256i k_shifted = _mm256_xor_si256(k, v_sign_bit);
            __m256i mask_k_lt = _mm256_cmpgt_epi32(v_threshold_shifted, k_shifted);

            // Condition 3: (keys[i] & 7) == 0
            __m256i k_and_7 = _mm256_and_si256(k, v_seven);
            __m256i mask_k_mod = _mm256_cmpeq_epi32(k_and_7, v_zero);

            // Combine masks
            __m256i mask = _mm256_and_si256(mask_f, _mm256_and_si256(mask_k_lt, mask_k_mod));

            // Apply mask to payloads
            __m256i masked_p = _mm256_and_si256(p, mask);

            // Horizontal sum of 8 uint32s into uint64
            // Extract low and high 128-bit parts
            __m128i low = _mm256_castsi256_si128(masked_p);
            __m128i high = _mm256_extracti128_si256(masked_p, 1);
            
            // Convert to 64-bit integers to avoid overflow during sum
            __m256i p64_1 = _mm256_cvtepu32_epi64(low);
            __m256i p64_2 = _mm256_cvtepu32_epi64(high);
            
            __m256i sum64 = _mm256_add_epi64(p64_1, p64_2);
            
            // Final horizontal sum of the 4 uint64s in sum64
            alignas(32) uint64_t temp[4];
            _mm256_store_si256(reinterpret_cast<__m256i*>(temp), sum64);
            current_iter_sum += (temp[0] + temp[1] + temp[2] + temp[3]);
        }

        // Tail processing
        for (; i < n; ++i) {
            if (f_ptr[i] && k_ptr[i] < threshold && ((k_ptr[i] & 7u) == 0u)) {
                current_iter_sum += static_cast<uint64_t>(p_ptr[i]);
            }
        }
        final_sum = current_iter_sum;
    }

    return final_sum;
}