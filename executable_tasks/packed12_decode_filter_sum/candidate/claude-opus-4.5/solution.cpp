#include "interface.h"

#include <cstdint>
#include <cstddef>

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
#include <immintrin.h>
#define HAS_SSE4 1
#else
#define HAS_SSE4 0
#endif

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    const uint8_t* __restrict data = packed.data();
    const std::size_t size = packed.size();
    uint64_t sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        std::size_t i = 0;

#if HAS_SSE4
        // Process 24 bytes at a time (16 12-bit values)
        // Each 3 bytes gives 2 values, so 24 bytes = 16 values
        const std::size_t simd_end = (size >= 23) ? (size - 23) : 0;
        
        __m128i thresh_vec = _mm_set1_epi16(static_cast<int16_t>(threshold));
        __m128i sum_lo = _mm_setzero_si128();
        __m128i sum_hi = _mm_setzero_si128();
        
        while (i <= simd_end) {
            // Load 24 bytes
            __m128i chunk0 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data + i));
            // Load remaining 8 bytes (we only need bytes 16-23)
            uint64_t tail;
            __builtin_memcpy(&tail, data + i + 16, 8);
            __m128i chunk1 = _mm_set_epi64x(0, static_cast<int64_t>(tail));
            
            // Extract 16 12-bit values from 24 bytes
            // Bytes 0,1,2 -> values 0,1
            // Bytes 3,4,5 -> values 2,3
            // etc.
            
            // Process in groups of 3 bytes -> 2 values
            // We'll extract each value individually for clarity, then optimize
            
            uint16_t vals[16];
            const uint8_t* p = data + i;
            
            for (int j = 0; j < 8; ++j) {
                const uint8_t* bp = p + j * 3;
                vals[j * 2] = static_cast<uint16_t>(bp[0]) | (static_cast<uint16_t>(bp[1] & 0x0F) << 8);
                vals[j * 2 + 1] = static_cast<uint16_t>(bp[1] >> 4) | (static_cast<uint16_t>(bp[2]) << 4);
            }
            
            // Load into SSE registers
            __m128i v0 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(vals));
            __m128i v1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(vals + 8));
            
            // Compare: we want v > threshold, which is NOT(v <= threshold)
            // _mm_cmpgt_epi16 does signed comparison, but our values are 0-4095 (12-bit)
            // and threshold is 0-65535, so we need unsigned comparison
            // For unsigned: a > b iff (a ^ 0x8000) > (b ^ 0x8000) in signed
            __m128i bias = _mm_set1_epi16(static_cast<int16_t>(0x8000));
            __m128i v0_biased = _mm_xor_si128(v0, bias);
            __m128i v1_biased = _mm_xor_si128(v1, bias);
            __m128i thresh_biased = _mm_xor_si128(thresh_vec, bias);
            
            __m128i mask0 = _mm_cmpgt_epi16(v0_biased, thresh_biased);
            __m128i mask1 = _mm_cmpgt_epi16(v1_biased, thresh_biased);
            
            // Apply mask
            __m128i masked0 = _mm_and_si128(v0, mask0);
            __m128i masked1 = _mm_and_si128(v1, mask1);
            
            // Horizontal sum - extend to 32-bit first
            // _mm_cvtepu16_epi32 extends lower 4 uint16 to 4 uint32
            __m128i ext0_lo = _mm_cvtepu16_epi32(masked0);
            __m128i ext0_hi = _mm_cvtepu16_epi32(_mm_srli_si128(masked0, 8));
            __m128i ext1_lo = _mm_cvtepu16_epi32(masked1);
            __m128i ext1_hi = _mm_cvtepu16_epi32(_mm_srli_si128(masked1, 8));
            
            sum_lo = _mm_add_epi32(sum_lo, ext0_lo);
            sum_lo = _mm_add_epi32(sum_lo, ext0_hi);
            sum_hi = _mm_add_epi32(sum_hi, ext1_lo);
            sum_hi = _mm_add_epi32(sum_hi, ext1_hi);
            
            i += 24;
        }
        
        // Reduce SIMD accumulators
        __m128i total = _mm_add_epi32(sum_lo, sum_hi);
        // Horizontal sum of 4 32-bit integers
        total = _mm_add_epi32(total, _mm_srli_si128(total, 8));
        total = _mm_add_epi32(total, _mm_srli_si128(total, 4));
        sum = static_cast<uint32_t>(_mm_cvtsi128_si32(total));
#endif

        // Scalar fallback for remaining bytes
        const std::size_t scalar_end = (size >= 2) ? (size - 2) : 0;
        while (i <= scalar_end) {
            const uint32_t a = static_cast<uint32_t>(data[i]) |
                              (static_cast<uint32_t>(data[i + 1] & 0x0F) << 8);
            const uint32_t b = static_cast<uint32_t>(data[i + 1] >> 4) |
                              (static_cast<uint32_t>(data[i + 2]) << 4);
            
            if (a > threshold) sum += a;
            if (b > threshold) sum += b;
            
            i += 3;
        }
    }

    return sum;
}