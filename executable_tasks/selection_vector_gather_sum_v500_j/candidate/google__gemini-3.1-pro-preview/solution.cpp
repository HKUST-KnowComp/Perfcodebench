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

    for (int iter = 0; iter < iters; ++iter) {
        __m256i acc = _mm256_setzero_si256();
        __m256i v_threshold = _mm256_set1_epi32(static_cast<int>(threshold));
        __m256i v_seven = _mm256_set1_epi32(7);
        __m256i v_zero = _mm256_setzero_si256();

        size_t i = 0;
        // Process 8 elements at a time using AVX2
        for (; i + 7 < n; i += 8) {
            __m256i k = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(k_ptr + i));
            __m128i f_small = _mm_loadu_si128(reinterpret_cast<const __m128i*>(f_ptr + i));
            __m256i f = _mm256_cvtepu8_epi32(f_small);
            __m256i p = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(p_ptr + i));

            // Condition 1: keys[i] < threshold
            // _mm256_cmpgt_epi32 returns true if param1 > param2. 
            // Since we need k < threshold, we use cmpgt(threshold, k).
            __m256i m_thresh = _mm256_cmpgt_epi32(v_threshold, k);

            // Condition 2: flags[i] != 0
            __m256i m_flag = _mm256_cmpgt_epi32(f, v_zero);

            // Condition 3: (keys[i] & 7) == 0
            __m256i m_mod8 = _mm256_cmpeq_epi32(_mm256_and_si256(k, v_seven), v_zero);

            // Combine masks
            __m256i mask = _mm256_and_si256(_mm256_and_si256(m_thresh, m_flag), m_mod8);

            // Apply mask to payloads
            __m256i filtered_p = _mm256_and_si256(mask, p);

            // Accumulate into 64-bit integers to avoid overflow
            __m256i p_low = _mm256_unpacklo_epi32(filtered_p, v_zero);
            __m256i p_high = _mm256_unpackhi_epi32(filtered_p, v_zero);
            
            acc = _mm256_add_epi64(acc, p_low);
            acc = _mm256_add_epi64(acc, p_high);
        }

        // Horizontal sum of the 4 uint64_t in the accumulator
        uint64_t temp_sum[4];
        _mm256_storeu_si256(reinterpret_cast<__m256i*>(temp_sum), acc);
        uint64_t current_sum = temp_sum[0] + temp_sum[1] + temp_sum[2] + temp_sum[3];

        // Handle remaining elements
        for (; i < n; ++i) {
            if (f_ptr[i] && k_ptr[i] < threshold && ((k_ptr[i] & 7u) == 0u)) {
                current_sum += static_cast<uint64_t>(p_ptr[i]);
            }
        }
        final_sum = current_sum;
    }

    return final_sum;
}