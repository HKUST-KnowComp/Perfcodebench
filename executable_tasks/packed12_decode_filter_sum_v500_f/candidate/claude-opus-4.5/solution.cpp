#include "interface.h"

#include <cstdint>
#include <cstddef>

#if defined(__AVX2__)
#include <immintrin.h>
#endif

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    const uint8_t* __restrict data = packed.data();
    const size_t size = packed.size();
    uint64_t sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        size_t i = 0;

#if defined(__AVX2__)
        // Process 24 bytes at a time (16 values)
        const __m256i thresh_vec = _mm256_set1_epi16(static_cast<int16_t>(threshold));
        __m256i sum_lo = _mm256_setzero_si256();
        __m256i sum_hi = _mm256_setzero_si256();
        
        // Shuffle mask to extract 12-bit values from packed bytes
        // For each 3 bytes [b0, b1, b2]: val0 = b0 | ((b1 & 0x0F) << 8), val1 = (b1 >> 4) | (b2 << 4)
        // Process 24 bytes -> 16 values
        
        while (i + 24 <= size) {
            // Load 24 bytes (we'll load 32 and ignore last 8)
            __m256i raw = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data + i));
            
            // Extract bytes manually for precise control
            // Bytes 0-23 contain our data
            // Pattern repeats every 3 bytes
            
            // For simplicity and correctness, decode scalar-style but accumulate in SIMD
            uint32_t vals[16];
            for (int j = 0; j < 8; ++j) {
                size_t base = i + j * 3;
                uint32_t b0 = data[base];
                uint32_t b1 = data[base + 1];
                uint32_t b2 = data[base + 2];
                vals[j * 2] = b0 | ((b1 & 0x0F) << 8);
                vals[j * 2 + 1] = (b1 >> 4) | (b2 << 4);
            }
            
            // Load into SIMD registers as 16-bit values
            __m256i v = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(vals));
            // Pack to 16-bit for comparison (values are 12-bit, fit in 16-bit)
            __m128i lo_128 = _mm256_castsi256_si128(v);
            __m128i hi_128 = _mm256_extracti128_si256(v, 1);
            // vals are 32-bit, need to handle differently
            
            // Actually, let's just do scalar accumulation with the decoded values
            for (int j = 0; j < 16; ++j) {
                if (vals[j] > threshold) {
                    sum += vals[j];
                }
            }
            
            i += 24;
        }
#endif

        // Scalar fallback / remainder
        while (i + 3 <= size) {
            uint32_t b0 = data[i];
            uint32_t b1 = data[i + 1];
            uint32_t b2 = data[i + 2];
            
            uint32_t a = b0 | ((b1 & 0x0Fu) << 8);
            uint32_t b = (b1 >> 4) | (b2 << 4);
            
            if (a > threshold) sum += a;
            if (b > threshold) sum += b;
            
            i += 3;
        }
    }

    return sum;
}