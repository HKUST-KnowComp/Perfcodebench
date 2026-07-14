#include "interface.h"

#include <cstdint>
#include <vector>

#ifdef __SSE2__
#include <emmintrin.h>
#endif

#ifdef __AVX2__
#include <immintrin.h>
#endif

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
    uint64_t sum = 0;
    const std::size_t size = packed.size();
    const uint8_t* data = packed.data();
    
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        std::size_t i = 0;
        
#ifdef __AVX2__
        // Process 32 bytes (64 nibbles) at a time with AVX2
        const std::size_t simd_end = size - (size % 32);
        const __m256i threshold_vec = _mm256_set1_epi8(threshold);
        const __m256i mask_low = _mm256_set1_epi8(0x0F);
        
        for (; i < simd_end; i += 32) {
            __m256i bytes = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data + i));
            
            // Extract low nibbles
            __m256i low_nibbles = _mm256_and_si256(bytes, mask_low);
            // Extract high nibbles
            __m256i high_nibbles = _mm256_and_si256(_mm256_srli_epi16(bytes, 4), mask_low);
            
            // Compare with threshold (unsigned)
            __m256i low_mask = _mm256_cmpgt_epi8(low_nibbles, threshold_vec);
            __m256i high_mask = _mm256_cmpgt_epi8(high_nibbles, threshold_vec);
            
            // Apply masks and accumulate
            __m256i low_filtered = _mm256_and_si256(low_nibbles, low_mask);
            __m256i high_filtered = _mm256_and_si256(high_nibbles, high_mask);
            
            // Horizontal sum - extract to scalar
            alignas(32) uint8_t low_arr[32];
            alignas(32) uint8_t high_arr[32];
            _mm256_store_si256(reinterpret_cast<__m256i*>(low_arr), low_filtered);
            _mm256_store_si256(reinterpret_cast<__m256i*>(high_arr), high_filtered);
            
            for (int j = 0; j < 32; ++j) {
                sum += low_arr[j];
                sum += high_arr[j];
            }
        }
#elif defined(__SSE2__)
        // Process 16 bytes (32 nibbles) at a time with SSE2
        const std::size_t simd_end = size - (size % 16);
        const __m128i threshold_vec = _mm_set1_epi8(threshold);
        const __m128i mask_low = _mm_set1_epi8(0x0F);
        
        for (; i < simd_end; i += 16) {
            __m128i bytes = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data + i));
            
            // Extract low nibbles
            __m128i low_nibbles = _mm_and_si128(bytes, mask_low);
            // Extract high nibbles
            __m128i high_nibbles = _mm_and_si128(_mm_srli_epi16(bytes, 4), mask_low);
            
            // Compare with threshold
            __m128i low_mask = _mm_cmpgt_epi8(low_nibbles, threshold_vec);
            __m128i high_mask = _mm_cmpgt_epi8(high_nibbles, threshold_vec);
            
            // Apply masks
            __m128i low_filtered = _mm_and_si128(low_nibbles, low_mask);
            __m128i high_filtered = _mm_and_si128(high_nibbles, high_mask);
            
            // Extract to scalar
            alignas(16) uint8_t low_arr[16];
            alignas(16) uint8_t high_arr[16];
            _mm_store_si128(reinterpret_cast<__m128i*>(low_arr), low_filtered);
            _mm_store_si128(reinterpret_cast<__m128i*>(high_arr), high_filtered);
            
            for (int j = 0; j < 16; ++j) {
                sum += low_arr[j];
                sum += high_arr[j];
            }
        }
#endif
        
        // Process remaining bytes with unrolled scalar code
        const std::size_t unroll_end = size - (size % 4);
        for (; i < unroll_end; i += 4) {
            uint8_t b0 = data[i];
            uint8_t b1 = data[i + 1];
            uint8_t b2 = data[i + 2];
            uint8_t b3 = data[i + 3];
            
            uint8_t low0 = b0 & 15u;
            uint8_t high0 = b0 >> 4u;
            uint8_t low1 = b1 & 15u;
            uint8_t high1 = b1 >> 4u;
            uint8_t low2 = b2 & 15u;
            uint8_t high2 = b2 >> 4u;
            uint8_t low3 = b3 & 15u;
            uint8_t high3 = b3 >> 4u;
            
            sum += (low0 > threshold) ? low0 : 0;
            sum += (high0 > threshold) ? high0 : 0;
            sum += (low1 > threshold) ? low1 : 0;
            sum += (high1 > threshold) ? high1 : 0;
            sum += (low2 > threshold) ? low2 : 0;
            sum += (high2 > threshold) ? high2 : 0;
            sum += (low3 > threshold) ? low3 : 0;
            sum += (high3 > threshold) ? high3 : 0;
        }
        
        // Handle remaining bytes
        for (; i < size; ++i) {
            uint8_t byte = data[i];
            uint8_t low = byte & 15u;
            uint8_t high = byte >> 4u;
            
            if (low > threshold) {
                sum += low;
            }
            if (high > threshold) {
                sum += high;
            }
        }
    }
    
    return sum;
}
