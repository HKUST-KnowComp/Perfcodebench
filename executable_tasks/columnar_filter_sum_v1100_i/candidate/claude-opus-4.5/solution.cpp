#include "interface.h"

#include <cstdint>
#include <cstddef>

#if defined(__AVX2__)
#include <immintrin.h>
#endif

uint64_t filter_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint8_t>& flags,
    uint32_t key_threshold,
    int iters) {
    
    const size_t n = keys.size();
    const uint32_t* __restrict k_ptr = keys.data();
    const uint32_t* __restrict v_ptr = values.data();
    const uint8_t* __restrict f_ptr = flags.data();
    
    uint64_t sum = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        
#if defined(__AVX2__)
        // Process 8 elements at a time using AVX2
        const __m256i thresh_vec = _mm256_set1_epi32(static_cast<int32_t>(key_threshold));
        const __m256i mask3_vec = _mm256_set1_epi32(3);
        const __m256i zero_vec = _mm256_setzero_si256();
        
        __m256i sum_lo = _mm256_setzero_si256();
        __m256i sum_hi = _mm256_setzero_si256();
        
        size_t i = 0;
        const size_t simd_end = (n >= 7) ? (n - 7) : 0;
        
        for (; i < simd_end; i += 8) {
            // Load 8 keys
            __m256i keys_v = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(k_ptr + i));
            
            // Load 8 values
            __m256i vals_v = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(v_ptr + i));
            
            // Load 8 flags (as bytes), expand to 32-bit
            __m128i flags_bytes = _mm_loadl_epi64(reinterpret_cast<const __m128i*>(f_ptr + i));
            __m256i flags_v = _mm256_cvtepu8_epi32(flags_bytes);
            
            // Condition 1: flags[i] != 0
            __m256i cond_flags = _mm256_cmpgt_epi32(flags_v, zero_vec);
            
            // Condition 2: keys[i] < key_threshold (unsigned comparison via subtraction trick)
            // For unsigned: a < b iff (a - b) has high bit set when b > a
            // Use max_epu32: if max(keys, thresh) == thresh, then keys < thresh
            __m256i max_kt = _mm256_max_epu32(keys_v, thresh_vec);
            __m256i cond_lt = _mm256_cmpeq_epi32(max_kt, thresh_vec);
            // But we need strict less than, not less-or-equal
            // keys < thresh iff keys != thresh AND max(keys, thresh) == thresh
            __m256i cond_eq = _mm256_cmpeq_epi32(keys_v, thresh_vec);
            cond_lt = _mm256_andnot_si256(cond_eq, cond_lt);
            
            // Condition 3: (keys[i] & 3) == 0
            __m256i and3 = _mm256_and_si256(keys_v, mask3_vec);
            __m256i cond_mod = _mm256_cmpeq_epi32(and3, zero_vec);
            
            // Combine all conditions
            __m256i mask = _mm256_and_si256(cond_flags, _mm256_and_si256(cond_lt, cond_mod));
            
            // Zero out values that don't pass the filter
            __m256i filtered = _mm256_and_si256(vals_v, mask);
            
            // Accumulate: extend 32-bit to 64-bit and add
            // Split into low and high 128-bit lanes
            __m128i filt_lo = _mm256_castsi256_si128(filtered);
            __m128i filt_hi = _mm256_extracti128_si256(filtered, 1);
            
            // Zero-extend to 64-bit
            __m256i ext_lo = _mm256_cvtepu32_epi64(filt_lo);
            __m256i ext_hi = _mm256_cvtepu32_epi64(filt_hi);
            
            sum_lo = _mm256_add_epi64(sum_lo, ext_lo);
            sum_hi = _mm256_add_epi64(sum_hi, ext_hi);
        }
        
        // Horizontal sum of accumulators
        __m256i total = _mm256_add_epi64(sum_lo, sum_hi);
        __m128i total_lo = _mm256_castsi256_si128(total);
        __m128i total_hi = _mm256_extracti128_si256(total, 1);
        __m128i sum128 = _mm_add_epi64(total_lo, total_hi);
        sum128 = _mm_add_epi64(sum128, _mm_srli_si128(sum128, 8));
        sum = static_cast<uint64_t>(_mm_cvtsi128_si64(sum128));
        
        // Scalar tail
        for (; i < n; ++i) {
            uint32_t key = k_ptr[i];
            if (f_ptr[i] && key < key_threshold && ((key & 3u) == 0u)) {
                sum += static_cast<uint64_t>(v_ptr[i]);
            }
        }
#else
        // Scalar fallback - fused filter and sum (no intermediate vector)
        for (size_t i = 0; i < n; ++i) {
            uint32_t key = k_ptr[i];
            if (f_ptr[i] && key < key_threshold && ((key & 3u) == 0u)) {
                sum += static_cast<uint64_t>(v_ptr[i]);
            }
        }
#endif
    }
    
    return sum;
}