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
        __m256i sum_vec = _mm256_setzero_si256();
        const __m256i threshold_vec = _mm256_set1_epi32(static_cast<int32_t>(key_threshold));
        const __m256i mask3_vec = _mm256_set1_epi32(3);
        const __m256i zero_vec = _mm256_setzero_si256();
        
        std::size_t i = 0;
        const std::size_t simd_end = (n >= 8) ? (n - 7) : 0;
        
        for (; i < simd_end; i += 8) {
            // Load 8 keys
            __m256i k = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(keys_ptr + i));
            
            // Load 8 flags (as bytes), expand to 32-bit
            __m128i flags_8 = _mm_loadl_epi64(reinterpret_cast<const __m128i*>(flags_ptr + i));
            __m256i f = _mm256_cvtepu8_epi32(flags_8);
            
            // Load 8 values
            __m256i v = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(values_ptr + i));
            
            // Condition 1: flags[i] != 0
            __m256i cond_flags = _mm256_cmpgt_epi32(f, zero_vec);
            
            // Condition 2: keys[i] < key_threshold (unsigned comparison via signed trick)
            // For unsigned comparison: a < b iff (a ^ 0x80000000) < (b ^ 0x80000000) as signed
            const __m256i sign_bit = _mm256_set1_epi32(static_cast<int32_t>(0x80000000u));
            __m256i k_signed = _mm256_xor_si256(k, sign_bit);
            __m256i thresh_signed = _mm256_xor_si256(threshold_vec, sign_bit);
            __m256i cond_thresh = _mm256_cmpgt_epi32(thresh_signed, k_signed);
            
            // Condition 3: (keys[i] & 3) == 0
            __m256i k_and_3 = _mm256_and_si256(k, mask3_vec);
            __m256i cond_mod4 = _mm256_cmpeq_epi32(k_and_3, zero_vec);
            
            // Combine all conditions
            __m256i mask = _mm256_and_si256(cond_flags, _mm256_and_si256(cond_thresh, cond_mod4));
            
            // Zero out non-qualifying values
            __m256i masked_v = _mm256_and_si256(v, mask);
            
            // Accumulate (widen to 64-bit to avoid overflow)
            __m256i lo = _mm256_unpacklo_epi32(masked_v, zero_vec);
            __m256i hi = _mm256_unpackhi_epi32(masked_v, zero_vec);
            sum_vec = _mm256_add_epi64(sum_vec, lo);
            sum_vec = _mm256_add_epi64(sum_vec, hi);
        }
        
        // Horizontal sum of sum_vec (4 x 64-bit)
        __m128i sum_lo = _mm256_castsi256_si128(sum_vec);
        __m128i sum_hi = _mm256_extracti128_si256(sum_vec, 1);
        __m128i sum_128 = _mm_add_epi64(sum_lo, sum_hi);
        sum_128 = _mm_add_epi64(sum_128, _mm_srli_si128(sum_128, 8));
        sum = static_cast<uint64_t>(_mm_cvtsi128_si64(sum_128));
        
        // Handle remaining elements
        for (; i < n; ++i) {
            if (flags_ptr[i] && keys_ptr[i] < key_threshold && ((keys_ptr[i] & 3u) == 0u)) {
                sum += static_cast<uint64_t>(values_ptr[i]);
            }
        }
#else
        // Scalar fallback - fused filter and sum
        for (std::size_t i = 0; i < n; ++i) {
            if (flags_ptr[i] && keys_ptr[i] < key_threshold && ((keys_ptr[i] & 3u) == 0u)) {
                sum += static_cast<uint64_t>(values_ptr[i]);
            }
        }
#endif
    }
    
    return sum;
}