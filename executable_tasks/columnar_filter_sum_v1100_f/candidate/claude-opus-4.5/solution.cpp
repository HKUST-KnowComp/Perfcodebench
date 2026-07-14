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
    
#if defined(__AVX2__)
    const __m256i thresh_vec = _mm256_set1_epi32(static_cast<int32_t>(key_threshold));
    const __m256i mask3_vec = _mm256_set1_epi32(3);
    const __m256i zero_vec = _mm256_setzero_si256();
    
    for (int iter = 0; iter < iters; ++iter) {
        __m256i sum_lo = _mm256_setzero_si256();
        __m256i sum_hi = _mm256_setzero_si256();
        
        size_t i = 0;
        
        // Process 8 elements at a time
        for (; i + 8 <= n; i += 8) {
            // Load 8 keys
            __m256i k = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(k_ptr + i));
            
            // Load 8 values
            __m256i v = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(v_ptr + i));
            
            // Load 8 flags (as bytes), then expand to 32-bit
            __m128i f8 = _mm_loadl_epi64(reinterpret_cast<const __m128i*>(f_ptr + i));
            __m256i f32 = _mm256_cvtepu8_epi32(f8);
            
            // flags != 0
            __m256i flag_mask = _mm256_cmpgt_epi32(f32, zero_vec);
            
            // keys < key_threshold (unsigned comparison via bias trick)
            // For unsigned compare a < b: (a ^ 0x80000000) < (b ^ 0x80000000) as signed
            const __m256i bias = _mm256_set1_epi32(static_cast<int32_t>(0x80000000u));
            __m256i k_biased = _mm256_xor_si256(k, bias);
            __m256i thresh_biased = _mm256_xor_si256(thresh_vec, bias);
            __m256i lt_mask = _mm256_cmpgt_epi32(thresh_biased, k_biased);
            
            // (keys & 3) == 0
            __m256i k_and_3 = _mm256_and_si256(k, mask3_vec);
            __m256i mod_mask = _mm256_cmpeq_epi32(k_and_3, zero_vec);
            
            // Combine all conditions
            __m256i final_mask = _mm256_and_si256(flag_mask, _mm256_and_si256(lt_mask, mod_mask));
            
            // Zero out values that don't pass the filter
            __m256i masked_v = _mm256_and_si256(v, final_mask);
            
            // Accumulate: extend 32-bit to 64-bit and add
            // Split into low and high 4 elements
            __m128i v_lo = _mm256_castsi256_si128(masked_v);
            __m128i v_hi = _mm256_extracti128_si256(masked_v, 1);
            
            // Zero-extend to 64-bit
            __m256i v64_lo = _mm256_cvtepu32_epi64(v_lo);
            __m256i v64_hi = _mm256_cvtepu32_epi64(v_hi);
            
            sum_lo = _mm256_add_epi64(sum_lo, v64_lo);
            sum_hi = _mm256_add_epi64(sum_hi, v64_hi);
        }
        
        // Horizontal sum of accumulators
        __m256i total = _mm256_add_epi64(sum_lo, sum_hi);
        __m128i total_lo = _mm256_castsi256_si128(total);
        __m128i total_hi = _mm256_extracti128_si256(total, 1);
        __m128i sum128 = _mm_add_epi64(total_lo, total_hi);
        sum128 = _mm_add_epi64(sum128, _mm_srli_si128(sum128, 8));
        sum = static_cast<uint64_t>(_mm_cvtsi128_si64(sum128));
        
        // Handle remaining elements
        for (; i < n; ++i) {
            if (f_ptr[i] && k_ptr[i] < key_threshold && ((k_ptr[i] & 3u) == 0u)) {
                sum += static_cast<uint64_t>(v_ptr[i]);
            }
        }
    }
#else
    // Scalar fallback - fused filter and sum (no intermediate vector)
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        for (size_t i = 0; i < n; ++i) {
            if (f_ptr[i] && k_ptr[i] < key_threshold && ((k_ptr[i] & 3u) == 0u)) {
                sum += static_cast<uint64_t>(v_ptr[i]);
            }
        }
    }
#endif
    
    return sum;
}
