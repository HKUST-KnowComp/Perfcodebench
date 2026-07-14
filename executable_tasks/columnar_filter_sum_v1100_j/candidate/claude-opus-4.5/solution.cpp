#include "interface.h"

#include <cstdint>
#include <immintrin.h>

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
        
        // AVX2 vectorized path - process 8 uint32_t at a time
        const __m256i threshold_vec = _mm256_set1_epi32(static_cast<int32_t>(key_threshold));
        const __m256i mask3_vec = _mm256_set1_epi32(3);
        const __m256i zero_vec = _mm256_setzero_si256();
        
        __m256i sum_lo = _mm256_setzero_si256(); // accumulate low 32 bits as 64-bit
        __m256i sum_hi = _mm256_setzero_si256(); // accumulate high 32 bits as 64-bit
        
        std::size_t i = 0;
        const std::size_t vec_end = (n >= 7) ? (n - 7) : 0;
        
        for (; i < vec_end; i += 8) {
            // Prefetch ahead
            if (i + 64 < n) {
                _mm_prefetch(reinterpret_cast<const char*>(keys_ptr + i + 64), _MM_HINT_T0);
                _mm_prefetch(reinterpret_cast<const char*>(values_ptr + i + 64), _MM_HINT_T0);
                _mm_prefetch(reinterpret_cast<const char*>(flags_ptr + i + 64), _MM_HINT_T0);
            }
            
            // Load 8 keys
            __m256i k = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(keys_ptr + i));
            
            // Load 8 values
            __m256i v = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(values_ptr + i));
            
            // Load 8 flags (as bytes, need to expand)
            uint64_t flags_8 = *reinterpret_cast<const uint64_t*>(flags_ptr + i);
            
            // Expand flags to 32-bit mask: flag != 0
            __m256i flags_expanded = _mm256_set_epi32(
                (flags_8 >> 56) & 0xFF ? -1 : 0,
                (flags_8 >> 48) & 0xFF ? -1 : 0,
                (flags_8 >> 40) & 0xFF ? -1 : 0,
                (flags_8 >> 32) & 0xFF ? -1 : 0,
                (flags_8 >> 24) & 0xFF ? -1 : 0,
                (flags_8 >> 16) & 0xFF ? -1 : 0,
                (flags_8 >> 8) & 0xFF ? -1 : 0,
                flags_8 & 0xFF ? -1 : 0
            );
            
            // keys[i] < key_threshold (unsigned comparison via signed trick)
            // For unsigned compare a < b: subtract 0x80000000 from both, then signed compare
            __m256i bias = _mm256_set1_epi32(static_cast<int32_t>(0x80000000u));
            __m256i k_biased = _mm256_sub_epi32(k, bias);
            __m256i thresh_biased = _mm256_sub_epi32(threshold_vec, bias);
            __m256i cmp_lt = _mm256_cmpgt_epi32(thresh_biased, k_biased);
            
            // (keys[i] & 3) == 0
            __m256i k_and_3 = _mm256_and_si256(k, mask3_vec);
            __m256i cmp_mod = _mm256_cmpeq_epi32(k_and_3, zero_vec);
            
            // Combine all conditions
            __m256i mask = _mm256_and_si256(flags_expanded, _mm256_and_si256(cmp_lt, cmp_mod));
            
            // Mask the values
            __m256i masked_v = _mm256_and_si256(v, mask);
            
            // Accumulate as 64-bit to avoid overflow
            // Split into low and high 4 elements, zero-extend to 64-bit
            __m128i v_lo = _mm256_castsi256_si128(masked_v);
            __m128i v_hi = _mm256_extracti128_si256(masked_v, 1);
            
            __m256i v_lo_64 = _mm256_cvtepu32_epi64(v_lo);
            __m256i v_hi_64 = _mm256_cvtepu32_epi64(v_hi);
            
            sum_lo = _mm256_add_epi64(sum_lo, v_lo_64);
            sum_hi = _mm256_add_epi64(sum_hi, v_hi_64);
        }
        
        // Horizontal sum of the vector accumulators
        __m256i total = _mm256_add_epi64(sum_lo, sum_hi);
        __m128i total_lo = _mm256_castsi256_si128(total);
        __m128i total_hi = _mm256_extracti128_si256(total, 1);
        __m128i total_128 = _mm_add_epi64(total_lo, total_hi);
        sum = static_cast<uint64_t>(_mm_extract_epi64(total_128, 0)) + 
              static_cast<uint64_t>(_mm_extract_epi64(total_128, 1));
        
        // Scalar tail
        for (; i < n; ++i) {
            if (flags_ptr[i] && keys_ptr[i] < key_threshold && ((keys_ptr[i] & 3u) == 0u)) {
                sum += static_cast<uint64_t>(values_ptr[i]);
            }
        }
    }
    
    return sum;
}