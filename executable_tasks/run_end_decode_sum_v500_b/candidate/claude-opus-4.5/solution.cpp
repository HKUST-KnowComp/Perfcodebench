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
        // Use AVX2 to process 4 runs at a time
        __m256i vsum = _mm256_setzero_si256();
        
        std::size_t i = 0;
        uint32_t prev = 0;
        
        // Process 4 runs at a time
        const std::size_t simd_end = (n >= 4) ? (n - 3) : 0;
        
        while (i < simd_end) {
            // Load 4 run_ends and 4 values
            __m128i ends_128 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&run_ends[i]));
            __m128i vals_128 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&values[i]));
            
            // Compute lengths: [run_ends[i]-prev, run_ends[i+1]-run_ends[i], ...]
            // Shift ends right and insert prev at position 0
            __m128i prev_ends = _mm_alignr_epi8(ends_128, _mm_set1_epi32(prev), 12);
            __m128i lens_128 = _mm_sub_epi32(ends_128, prev_ends);
            
            // Zero-extend to 64-bit for multiplication
            __m256i lens_64 = _mm256_cvtepu32_epi64(lens_128);
            __m256i vals_64 = _mm256_cvtepu32_epi64(vals_128);
            
            // Multiply and accumulate
            __m256i products = _mm256_mul_epu32(lens_64, vals_64);
            vsum = _mm256_add_epi64(vsum, products);
            
            prev = run_ends[i + 3];
            i += 4;
        }
        
        // Horizontal sum of vsum
        __m128i vsum_lo = _mm256_castsi256_si128(vsum);
        __m128i vsum_hi = _mm256_extracti128_si256(vsum, 1);
        __m128i vsum_128 = _mm_add_epi64(vsum_lo, vsum_hi);
        vsum_128 = _mm_add_epi64(vsum_128, _mm_srli_si128(vsum_128, 8));
        sum = static_cast<uint64_t>(_mm_cvtsi128_si64(vsum_128));
        
        // Handle remaining runs
        for (; i < n; ++i) {
            const uint32_t len = run_ends[i] - prev;
            sum += static_cast<uint64_t>(len) * static_cast<uint64_t>(values[i]);
            prev = run_ends[i];
        }
#else
        // Scalar fallback: compute sum directly without decoding
        uint32_t prev = 0;
        for (std::size_t i = 0; i < n; ++i) {
            const uint32_t len = run_ends[i] - prev;
            sum += static_cast<uint64_t>(len) * static_cast<uint64_t>(values[i]);
            prev = run_ends[i];
        }
#endif
    }
    
    return sum;
}
