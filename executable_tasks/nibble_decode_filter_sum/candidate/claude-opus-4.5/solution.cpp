#include "interface.h"

#include <cstdint>
#include <vector>

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
#include <immintrin.h>
#define HAS_SSE4 1
#else
#define HAS_SSE4 0
#endif

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
    const uint8_t* data = packed.data();
    const std::size_t n = packed.size();
    uint64_t sum = 0;

#if HAS_SSE4
    const __m128i mask_lo = _mm_set1_epi8(0x0F);
    const __m128i thresh_vec = _mm_set1_epi8(static_cast<char>(threshold));
    
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        std::size_t i = 0;
        
        // Process 16 bytes at a time (32 nibbles)
        const std::size_t simd_end = (n >= 16) ? (n - 15) : 0;
        
        __m128i acc0 = _mm_setzero_si128();
        __m128i acc1 = _mm_setzero_si128();
        __m128i acc2 = _mm_setzero_si128();
        __m128i acc3 = _mm_setzero_si128();
        int acc_count = 0;
        
        for (; i < simd_end; i += 16) {
            __m128i bytes = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data + i));
            
            // Extract low and high nibbles
            __m128i lo = _mm_and_si128(bytes, mask_lo);
            __m128i hi = _mm_and_si128(_mm_srli_epi16(bytes, 4), mask_lo);
            
            // Compare > threshold (using signed comparison trick: a > b iff a - b - 1 >= 0 unsigned)
            // For unsigned: a > b iff max(a,b) != b
            __m128i lo_gt = _mm_cmpgt_epi8(_mm_xor_si128(lo, _mm_set1_epi8((char)0x80)), 
                                           _mm_xor_si128(thresh_vec, _mm_set1_epi8((char)0x80)));
            __m128i hi_gt = _mm_cmpgt_epi8(_mm_xor_si128(hi, _mm_set1_epi8((char)0x80)), 
                                           _mm_xor_si128(thresh_vec, _mm_set1_epi8((char)0x80)));
            
            // Mask values that don't pass threshold
            lo = _mm_and_si128(lo, lo_gt);
            hi = _mm_and_si128(hi, hi_gt);
            
            // Horizontal sum using SAD against zero
            __m128i sad_lo = _mm_sad_epu8(lo, _mm_setzero_si128());
            __m128i sad_hi = _mm_sad_epu8(hi, _mm_setzero_si128());
            
            acc0 = _mm_add_epi64(acc0, sad_lo);
            acc1 = _mm_add_epi64(acc1, sad_hi);
            
            acc_count++;
            if (acc_count >= 65535) {
                // Flush accumulators to prevent overflow (very unlikely with nibbles)
                sum += static_cast<uint64_t>(_mm_extract_epi64(acc0, 0));
                sum += static_cast<uint64_t>(_mm_extract_epi64(acc0, 1));
                sum += static_cast<uint64_t>(_mm_extract_epi64(acc1, 0));
                sum += static_cast<uint64_t>(_mm_extract_epi64(acc1, 1));
                acc0 = _mm_setzero_si128();
                acc1 = _mm_setzero_si128();
                acc_count = 0;
            }
        }
        
        // Flush remaining accumulator values
        sum += static_cast<uint64_t>(_mm_extract_epi64(acc0, 0));
        sum += static_cast<uint64_t>(_mm_extract_epi64(acc0, 1));
        sum += static_cast<uint64_t>(_mm_extract_epi64(acc1, 0));
        sum += static_cast<uint64_t>(_mm_extract_epi64(acc1, 1));
        
        // Handle remaining bytes
        for (; i < n; ++i) {
            uint8_t b = data[i];
            uint8_t lo = b & 0x0Fu;
            uint8_t hi = b >> 4u;
            if (lo > threshold) sum += lo;
            if (hi > threshold) sum += hi;
        }
    }
#else
    // Scalar fallback
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        for (std::size_t i = 0; i < n; ++i) {
            uint8_t b = data[i];
            uint8_t lo = b & 0x0Fu;
            uint8_t hi = b >> 4u;
            if (lo > threshold) sum += lo;
            if (hi > threshold) sum += hi;
        }
    }
#endif

    return sum;
}
