#include "interface.h"

#include <cstdint>
#include <immintrin.h>

uint64_t filter_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint8_t>& flags,
    uint32_t key_threshold,
    int iters) {
    
    const size_t n = keys.size();
    const uint32_t* __restrict keys_ptr = keys.data();
    const uint32_t* __restrict values_ptr = values.data();
    const uint8_t* __restrict flags_ptr = flags.data();
    
    uint64_t sum = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        
#if defined(__AVX2__)
        // AVX2 vectorized path
        const __m256i threshold_vec = _mm256_set1_epi32(static_cast<int32_t>(key_threshold));
        const __m256i mask3_vec = _mm256_set1_epi32(3);
        const __m256i zero_vec = _mm256_setzero_si256();
        
        __m256i sum_lo = _mm256_setzero_si256();
        __m256i sum_hi = _mm256_setzero_si256();
        
        size_t i = 0;
        const size_t vec_end = (n >= 8) ? (n - 7) : 0;
        
        for (; i < vec_end; i += 8) {
            // Prefetch ahead
            if (i + 64 < n) {
                _mm_prefetch(reinterpret_cast<const char*>(keys_ptr + i + 64), _MM_HINT_T0);
                _mm_prefetch(reinterpret_cast<const char*>(values_ptr + i + 64), _MM_HINT_T0);
            }
            
            // Load 8 keys
            __m256i k = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(keys_ptr + i));
            
            // Load 8 values
            __m256i v = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(values_ptr + i));
            
            // Load 8 flags (as bytes, need to expand)
            __m128i f8 = _mm_loadl_epi64(reinterpret_cast<const __m128i*>(flags_ptr + i));
            __m256i f32 = _mm256_cvtepu8_epi32(f8);
            
            // Condition 1: flags[i] != 0
            __m256i cond_flags = _mm256_cmpgt_epi32(f32, zero_vec);
            
            // Condition 2: keys[i] < key_threshold (unsigned comparison via signed trick)
            // For unsigned comparison: a < b iff (a ^ 0x80000000) < (b ^ 0x80000000) as signed
            const __m256i sign_bit = _mm256_set1_epi32(static_cast<int32_t>(0x80000000u));
            __m256i k_signed = _mm256_xor_si256(k, sign_bit);
            __m256i thresh_signed = _mm256_xor_si256(threshold_vec, sign_bit);
            __m256i cond_thresh = _mm256_cmpgt_epi32(thresh_signed, k_signed);
            
            // Condition 3: (keys[i] & 3) == 0
            __m256i k_and_3 = _mm256_and_si256(k, mask3_vec);
            __m256i cond_mod4 = _mm256_cmpeq_epi32(k_and_3, zero_vec);
            
            // Combined mask
            __m256i mask = _mm256_and_si256(cond_flags, _mm256_and_si256(cond_thresh, cond_mod4));
            
            // Masked values
            __m256i masked_v = _mm256_and_si256(v, mask);
            
            // Accumulate as 64-bit to avoid overflow
            // Split into low and high 4 elements
            __m256i v_lo = _mm256_cvtepu32_epi64(_mm256_castsi256_si128(masked_v));
            __m256i v_hi = _mm256_cvtepu32_epi64(_mm256_extracti128_si256(masked_v, 1));
            
            sum_lo = _mm256_add_epi64(sum_lo, v_lo);
            sum_hi = _mm256_add_epi64(sum_hi, v_hi);
        }
        
        // Horizontal sum of 64-bit accumulators
        __m256i total = _mm256_add_epi64(sum_lo, sum_hi);
        __m128i total_lo = _mm256_castsi256_si128(total);
        __m128i total_hi = _mm256_extracti128_si256(total, 1);
        __m128i sum128 = _mm_add_epi64(total_lo, total_hi);
        sum = static_cast<uint64_t>(_mm_extract_epi64(sum128, 0)) + 
              static_cast<uint64_t>(_mm_extract_epi64(sum128, 1));
        
        // Scalar tail
        for (; i < n; ++i) {
            if (flags_ptr[i] && keys_ptr[i] < key_threshold && ((keys_ptr[i] & 3u) == 0u)) {
                sum += static_cast<uint64_t>(values_ptr[i]);
            }
        }
#else
        // Scalar fallback with fused filter-sum
        for (size_t i = 0; i < n; ++i) {
            if (flags_ptr[i] && keys_ptr[i] < key_threshold && ((keys_ptr[i] & 3u) == 0u)) {
                sum += static_cast<uint64_t>(values_ptr[i]);
            }
        }
#endif
    }
    
    return sum;
}
