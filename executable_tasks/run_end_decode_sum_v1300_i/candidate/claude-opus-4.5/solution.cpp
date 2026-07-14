#include "interface.h"

#include <cstdint>
#include <vector>

#if defined(__x86_64__) || defined(_M_X64)
#include <immintrin.h>
#define HAS_AVX2 1
#else
#define HAS_AVX2 0
#endif

uint64_t run_end_sum(
    const std::vector<uint32_t>& run_ends,
    const std::vector<uint32_t>& values,
    int iters) {
    
    const std::size_t n = values.size();
    if (n == 0) {
        return 0;
    }
    
    uint64_t sum = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        
#if HAS_AVX2
        // Use SIMD to process 4 runs at a time
        const uint32_t* re_ptr = run_ends.data();
        const uint32_t* val_ptr = values.data();
        
        __m256i sum_vec = _mm256_setzero_si256();
        
        std::size_t i = 0;
        uint32_t prev = 0;
        
        // Process 4 runs at a time
        for (; i + 4 <= n; i += 4) {
            // Load 4 run_ends and 4 values
            __m128i ends = _mm_loadu_si128(reinterpret_cast<const __m128i*>(re_ptr + i));
            __m128i vals = _mm_loadu_si128(reinterpret_cast<const __m128i*>(val_ptr + i));
            
            // Compute lengths: [re[i]-prev, re[i+1]-re[i], re[i+2]-re[i+1], re[i+3]-re[i+2]]
            __m128i prev_vec = _mm_set_epi32(
                static_cast<int>(re_ptr[i + 2]),
                static_cast<int>(re_ptr[i + 1]),
                static_cast<int>(re_ptr[i]),
                static_cast<int>(prev)
            );
            __m128i lens = _mm_sub_epi32(ends, prev_vec);
            
            // Multiply values by lengths (need 64-bit result)
            // Extend to 64-bit and multiply
            __m256i vals_64 = _mm256_cvtepu32_epi64(vals);
            __m256i lens_64 = _mm256_cvtepu32_epi64(lens);
            
            // Multiply (64-bit)
            __m256i prod = _mm256_mul_epu32(vals_64, lens_64);
            // For the high parts
            __m256i vals_hi = _mm256_srli_epi64(vals_64, 32);
            __m256i lens_hi = _mm256_srli_epi64(lens_64, 32);
            // vals_64 and lens_64 are zero-extended, so high 32 bits are 0
            // _mm256_mul_epu32 multiplies the low 32 bits of each 64-bit lane
            // Since we zero-extended, this gives us the full 64-bit product
            
            sum_vec = _mm256_add_epi64(sum_vec, prod);
            
            prev = re_ptr[i + 3];
        }
        
        // Horizontal sum of sum_vec
        __m128i sum_lo = _mm256_castsi256_si128(sum_vec);
        __m128i sum_hi = _mm256_extracti128_si256(sum_vec, 1);
        __m128i sum128 = _mm_add_epi64(sum_lo, sum_hi);
        sum = static_cast<uint64_t>(_mm_extract_epi64(sum128, 0)) + 
              static_cast<uint64_t>(_mm_extract_epi64(sum128, 1));
        
        // Handle remaining runs
        for (; i < n; ++i) {
            uint32_t len = re_ptr[i] - prev;
            sum += static_cast<uint64_t>(val_ptr[i]) * len;
            prev = re_ptr[i];
        }
#else
        // Scalar fallback: compute sum directly without decoding
        uint32_t prev = 0;
        for (std::size_t i = 0; i < n; ++i) {
            uint32_t len = run_ends[i] - prev;
            sum += static_cast<uint64_t>(values[i]) * len;
            prev = run_ends[i];
        }
#endif
    }
    
    return sum;
}