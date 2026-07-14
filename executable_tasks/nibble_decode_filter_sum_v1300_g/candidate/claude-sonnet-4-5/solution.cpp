#include "interface.h"

#include <vector>
#include <cstdint>

#ifdef __SSE2__
#include <emmintrin.h>
#endif

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
    uint64_t sum = 0;
    const std::size_t size = packed.size();
    const uint8_t* data = packed.data();
    
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        
#ifdef __SSE2__
        // SIMD path: process 16 bytes at a time
        const std::size_t simd_end = size & ~15ULL;
        const __m128i threshold_vec = _mm_set1_epi8(threshold);
        
        for (std::size_t i = 0; i < simd_end; i += 16) {
            __m128i packed_data = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data + i));
            
            // Extract low nibbles
            __m128i low_nibbles = _mm_and_si128(packed_data, _mm_set1_epi8(0x0F));
            // Extract high nibbles
            __m128i high_nibbles = _mm_and_si128(_mm_srli_epi16(packed_data, 4), _mm_set1_epi8(0x0F));
            
            // Compare with threshold (unsigned)
            __m128i low_mask = _mm_cmpgt_epi8(low_nibbles, threshold_vec);
            __m128i high_mask = _mm_cmpgt_epi8(high_nibbles, threshold_vec);
            
            // Apply masks and accumulate
            __m128i low_filtered = _mm_and_si128(low_nibbles, low_mask);
            __m128i high_filtered = _mm_and_si128(high_nibbles, high_mask);
            
            // Horizontal sum using SAD (sum of absolute differences)
            __m128i zero = _mm_setzero_si128();
            __m128i low_sum = _mm_sad_epu8(low_filtered, zero);
            __m128i high_sum = _mm_sad_epu8(high_filtered, zero);
            
            // Extract 64-bit sums
            sum += _mm_cvtsi128_si64(low_sum);
            sum += _mm_cvtsi128_si64(_mm_unpackhi_epi64(low_sum, low_sum));
            sum += _mm_cvtsi128_si64(high_sum);
            sum += _mm_cvtsi128_si64(_mm_unpackhi_epi64(high_sum, high_sum));
        }
        
        // Scalar tail
        for (std::size_t i = simd_end; i < size; ++i) {
            uint8_t byte = data[i];
            uint8_t low = byte & 15u;
            uint8_t high = byte >> 4u;
            if (low > threshold) sum += low;
            if (high > threshold) sum += high;
        }
#else
        // Scalar path with unrolling
        std::size_t i = 0;
        const std::size_t unroll_end = size & ~3ULL;
        
        for (; i < unroll_end; i += 4) {
            uint8_t b0 = data[i];
            uint8_t b1 = data[i + 1];
            uint8_t b2 = data[i + 2];
            uint8_t b3 = data[i + 3];
            
            uint8_t l0 = b0 & 15u, h0 = b0 >> 4u;
            uint8_t l1 = b1 & 15u, h1 = b1 >> 4u;
            uint8_t l2 = b2 & 15u, h2 = b2 >> 4u;
            uint8_t l3 = b3 & 15u, h3 = b3 >> 4u;
            
            if (l0 > threshold) sum += l0;
            if (h0 > threshold) sum += h0;
            if (l1 > threshold) sum += l1;
            if (h1 > threshold) sum += h1;
            if (l2 > threshold) sum += l2;
            if (h2 > threshold) sum += h2;
            if (l3 > threshold) sum += l3;
            if (h3 > threshold) sum += h3;
        }
        
        for (; i < size; ++i) {
            uint8_t byte = data[i];
            uint8_t low = byte & 15u;
            uint8_t high = byte >> 4u;
            if (low > threshold) sum += low;
            if (high > threshold) sum += high;
        }
#endif
    }
    
    return sum;
}
