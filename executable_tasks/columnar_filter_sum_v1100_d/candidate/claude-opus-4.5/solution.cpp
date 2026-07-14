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
        __m256i sum_vec = _mm256_setzero_si256();
        const __m256i mask3 = _mm256_set1_epi32(3);
        const __m256i zero = _mm256_setzero_si256();
        const __m256i thresh = _mm256_set1_epi32(key_threshold);
        
        size_t i = 0;
        const size_t vec_end = (n >= 8) ? (n - 7) : 0;
        
        for (; i < vec_end; i += 8) {
            // Load 8 keys
            __m256i k = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(k_ptr + i));
            
            // Load 8 values
            __m256i v = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(v_ptr + i));
            
            // Load 8 flags (as bytes), zero-extend to 32-bit
            __m128i f8 = _mm_loadl_epi64(reinterpret_cast<const __m128i*>(f_ptr + i));
            __m256i f32 = _mm256_cvtepu8_epi32(f8);
            
            // flags != 0
            __m256i flag_mask = _mm256_cmpgt_epi32(f32, zero);
            
            // keys < key_threshold (unsigned comparison via bias trick)
            // For unsigned compare: a < b iff (a ^ 0x80000000) < (b ^ 0x80000000) as signed
            const __m256i bias = _mm256_set1_epi32(0x80000000u);
            __m256i k_biased = _mm256_xor_si256(k, bias);
            __m256i thresh_biased = _mm256_xor_si256(thresh, bias);
            __m256i lt_mask = _mm256_cmpgt_epi32(thresh_biased, k_biased);
            
            // (keys & 3) == 0
            __m256i k_and_3 = _mm256_and_si256(k, mask3);
            __m256i mod_mask = _mm256_cmpeq_epi32(k_and_3, zero);
            
            // Combine all masks
            __m256i combined = _mm256_and_si256(flag_mask, _mm256_and_si256(lt_mask, mod_mask));
            
            // Mask values and accumulate
            __m256i masked_v = _mm256_and_si256(v, combined);
            sum_vec = _mm256_add_epi64(sum_vec, 
                _mm256_add_epi64(
                    _mm256_cvtepu32_epi64(_mm256_castsi256_si128(masked_v)),
                    _mm256_cvtepu32_epi64(_mm256_extracti128_si256(masked_v, 1))
                ));
        }
        
        // Horizontal sum of sum_vec (4 x 64-bit)
        __m128i lo = _mm256_castsi256_si128(sum_vec);
        __m128i hi = _mm256_extracti128_si256(sum_vec, 1);
        __m128i sum128 = _mm_add_epi64(lo, hi);
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
        // Scalar fallback with loop unrolling
        size_t i = 0;
        const size_t unroll_end = (n >= 4) ? (n - 3) : 0;
        
        for (; i < unroll_end; i += 4) {
            uint32_t k0 = k_ptr[i], k1 = k_ptr[i+1], k2 = k_ptr[i+2], k3 = k_ptr[i+3];
            uint8_t f0 = f_ptr[i], f1 = f_ptr[i+1], f2 = f_ptr[i+2], f3 = f_ptr[i+3];
            
            if (f0 && k0 < key_threshold && ((k0 & 3u) == 0u)) sum += v_ptr[i];
            if (f1 && k1 < key_threshold && ((k1 & 3u) == 0u)) sum += v_ptr[i+1];
            if (f2 && k2 < key_threshold && ((k2 & 3u) == 0u)) sum += v_ptr[i+2];
            if (f3 && k3 < key_threshold && ((k3 & 3u) == 0u)) sum += v_ptr[i+3];
        }
        
        for (; i < n; ++i) {
            uint32_t key = k_ptr[i];
            if (f_ptr[i] && key < key_threshold && ((key & 3u) == 0u)) {
                sum += static_cast<uint64_t>(v_ptr[i]);
            }
        }
#endif
    }
    
    return sum;
}