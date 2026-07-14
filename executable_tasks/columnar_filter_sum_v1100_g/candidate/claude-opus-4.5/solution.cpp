#include "interface.h"

#include <cstdint>
#include <vector>

#if defined(__AVX2__)
#include <immintrin.h>
#endif

uint64_t filter_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint8_t>& flags,
    uint32_t key_threshold,
    int iters) {
    
    const std::size_t n = keys.size();
    const uint32_t* __restrict keys_ptr = keys.data();
    const uint32_t* __restrict values_ptr = values.data();
    const uint8_t* __restrict flags_ptr = flags.data();
    
    uint64_t sum = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        
#if defined(__AVX2__)
        __m256i sum_vec_lo = _mm256_setzero_si256();
        __m256i sum_vec_hi = _mm256_setzero_si256();
        
        const __m256i threshold_vec = _mm256_set1_epi32(static_cast<int32_t>(key_threshold));
        const __m256i mask3_vec = _mm256_set1_epi32(3);
        const __m256i zero_vec = _mm256_setzero_si256();
        
        std::size_t i = 0;
        const std::size_t simd_end = (n >= 8) ? (n - 7) : 0;
        
        for (; i < simd_end; i += 8) {
            // Load 8 keys
            __m256i k = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(keys_ptr + i));
            // Load 8 values
            __m256i v = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(values_ptr + i));
            
            // Load 8 flags and expand to 32-bit
            __m128i flags_8 = _mm_loadl_epi64(reinterpret_cast<const __m128i*>(flags_ptr + i));
            __m256i f = _mm256_cvtepu8_epi32(flags_8);
            
            // flags != 0
            __m256i flag_mask = _mm256_cmpgt_epi32(f, zero_vec);
            
            // keys < key_threshold (unsigned comparison via bias trick)
            // For unsigned comparison: a < b iff (a ^ 0x80000000) < (b ^ 0x80000000) as signed
            const __m256i bias = _mm256_set1_epi32(static_cast<int32_t>(0x80000000u));
            __m256i k_biased = _mm256_xor_si256(k, bias);
            __m256i thresh_biased = _mm256_xor_si256(threshold_vec, bias);
            __m256i lt_mask = _mm256_cmpgt_epi32(thresh_biased, k_biased);
            
            // (keys & 3) == 0
            __m256i k_and_3 = _mm256_and_si256(k, mask3_vec);
            __m256i mod_mask = _mm256_cmpeq_epi32(k_and_3, zero_vec);
            
            // Combine all conditions
            __m256i combined_mask = _mm256_and_si256(flag_mask, _mm256_and_si256(lt_mask, mod_mask));
            
            // Zero out values that don't pass the filter
            __m256i filtered_v = _mm256_and_si256(v, combined_mask);
            
            // Accumulate into 64-bit sums
            // Unpack to 64-bit and add
            __m256i v_lo = _mm256_unpacklo_epi32(filtered_v, zero_vec);
            __m256i v_hi = _mm256_unpackhi_epi32(filtered_v, zero_vec);
            
            sum_vec_lo = _mm256_add_epi64(sum_vec_lo, v_lo);
            sum_vec_hi = _mm256_add_epi64(sum_vec_hi, v_hi);
        }
        
        // Horizontal sum of the 64-bit accumulators
        __m256i total = _mm256_add_epi64(sum_vec_lo, sum_vec_hi);
        
        // Extract and sum the 4 64-bit values
        __m128i lo128 = _mm256_castsi256_si128(total);
        __m128i hi128 = _mm256_extracti128_si256(total, 1);
        __m128i sum128 = _mm_add_epi64(lo128, hi128);
        sum = static_cast<uint64_t>(_mm_extract_epi64(sum128, 0)) + 
              static_cast<uint64_t>(_mm_extract_epi64(sum128, 1));
        
        // Handle remaining elements
        for (; i < n; ++i) {
            uint32_t k = keys_ptr[i];
            if (flags_ptr[i] && k < key_threshold && ((k & 3u) == 0u)) {
                sum += static_cast<uint64_t>(values_ptr[i]);
            }
        }
#else
        // Scalar fallback - fused filter and sum without intermediate storage
        for (std::size_t i = 0; i < n; ++i) {
            uint32_t k = keys_ptr[i];
            if (flags_ptr[i] && k < key_threshold && ((k & 3u) == 0u)) {
                sum += static_cast<uint64_t>(values_ptr[i]);
            }
        }
#endif
    }
    
    return sum;
}