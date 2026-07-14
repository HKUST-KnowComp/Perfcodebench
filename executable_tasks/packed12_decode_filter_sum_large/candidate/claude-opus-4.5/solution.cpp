#include "interface.h"

#include <cstdint>
#include <cstddef>

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
#define USE_SIMD 1
#include <immintrin.h>
#else
#define USE_SIMD 0
#endif

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    const uint8_t* __restrict data = packed.data();
    const std::size_t size = packed.size();
    uint64_t sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        std::size_t i = 0;

#if USE_SIMD && defined(__AVX2__)
        // Process 48 bytes at a time (32 12-bit values)
        const __m256i mask12 = _mm256_set1_epi16(0x0FFF);
        const __m256i thresh_vec = _mm256_set1_epi16(static_cast<int16_t>(threshold));
        
        // Shuffle masks to extract 12-bit values from packed bytes
        // For each 48 bytes, we get 32 12-bit values
        // Process in chunks of 24 bytes -> 16 values
        
        while (i + 24 <= size) {
            // Load 24 bytes (gives us 16 12-bit values)
            // Layout: [b0 b1 b2] [b3 b4 b5] ... each triplet gives 2 values
            // v0 = b0 | (b1 & 0x0F) << 8
            // v1 = (b1 >> 4) | (b2 << 4)
            
            __m128i raw = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data + i));
            __m128i raw2 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data + i + 12));
            
            // Shuffle to align bytes for extraction
            // For values at positions 0,1: bytes 0,1,2
            // For values at positions 2,3: bytes 3,4,5
            // etc.
            
            // Extract first 8 values from first 12 bytes
            // Positions: 0,1,2 -> val0,val1; 3,4,5 -> val2,val3; 6,7,8 -> val4,val5; 9,10,11 -> val6,val7
            
            const __m128i shuf_lo = _mm_setr_epi8(
                0, 1, 1, 2, 3, 4, 4, 5, 6, 7, 7, 8, 9, 10, 10, 11
            );
            
            __m128i shuffled1 = _mm_shuffle_epi8(raw, shuf_lo);
            __m128i shuffled2 = _mm_shuffle_epi8(raw2, shuf_lo);
            
            // Now we have pairs like [b0, b1], [b1, b2], [b3, b4], [b4, b5], ...
            // For even positions (0,2,4,6): val = lo | ((hi & 0x0F) << 8)
            // For odd positions (1,3,5,7): val = (lo >> 4) | (hi << 4)
            
            // Combine into 256-bit register
            __m256i combined = _mm256_set_m128i(shuffled2, shuffled1);
            
            // Extract even values: mask with 0x0FFF after treating as 16-bit
            __m256i even_vals = _mm256_and_si256(combined, mask12);
            
            // Extract odd values: shift right by 4 bits
            __m256i odd_vals = _mm256_srli_epi16(combined, 4);
            odd_vals = _mm256_and_si256(odd_vals, mask12);
            
            // Now interleave even and odd to get correct order
            // even_vals has: v0, v2, v4, v6, v8, v10, v12, v14 (in wrong positions)
            // odd_vals has: v1, v3, v5, v7, v9, v11, v13, v15 (in wrong positions)
            
            // Actually the layout after shuffle is:
            // [b0,b1] [b1,b2] [b3,b4] [b4,b5] [b6,b7] [b7,b8] [b9,b10] [b10,b11]
            // So treating as 16-bit words:
            // word0 = b1*256 + b0, masked with 0x0FFF gives v0
            // word1 = b2*256 + b1, shifted right 4 gives v1
            // word2 = b4*256 + b3, masked gives v2
            // word3 = b5*256 + b4, shifted gives v3
            // etc.
            
            // Compare with threshold (unsigned comparison via saturating subtract)
            // val > threshold iff val - threshold - 1 doesn't underflow (for unsigned)
            // Use signed comparison after adjusting
            
            // For unsigned 16-bit comparison: a > b iff (a ^ 0x8000) > (b ^ 0x8000) signed
            const __m256i sign_bit = _mm256_set1_epi16(static_cast<short>(0x8000));
            
            __m256i even_xor = _mm256_xor_si256(even_vals, sign_bit);
            __m256i odd_xor = _mm256_xor_si256(odd_vals, sign_bit);
            __m256i thresh_xor = _mm256_xor_si256(thresh_vec, sign_bit);
            
            __m256i even_mask = _mm256_cmpgt_epi16(even_xor, thresh_xor);
            __m256i odd_mask = _mm256_cmpgt_epi16(odd_xor, thresh_xor);
            
            // Zero out values not above threshold
            even_vals = _mm256_and_si256(even_vals, even_mask);
            odd_vals = _mm256_and_si256(odd_vals, odd_mask);
            
            // Sum horizontally - extend to 32-bit first
            // Unpack to 32-bit and accumulate
            __m256i even_lo = _mm256_unpacklo_epi16(even_vals, _mm256_setzero_si256());
            __m256i even_hi = _mm256_unpackhi_epi16(even_vals, _mm256_setzero_si256());
            __m256i odd_lo = _mm256_unpacklo_epi16(odd_vals, _mm256_setzero_si256());
            __m256i odd_hi = _mm256_unpackhi_epi16(odd_vals, _mm256_setzero_si256());
            
            __m256i sum32 = _mm256_add_epi32(even_lo, even_hi);
            sum32 = _mm256_add_epi32(sum32, odd_lo);
            sum32 = _mm256_add_epi32(sum32, odd_hi);
            
            // Horizontal sum of 32-bit values
            __m128i sum32_lo = _mm256_castsi256_si128(sum32);
            __m128i sum32_hi = _mm256_extracti128_si256(sum32, 1);
            __m128i sum32_128 = _mm_add_epi32(sum32_lo, sum32_hi);
            
            // Further reduce
            sum32_128 = _mm_add_epi32(sum32_128, _mm_srli_si128(sum32_128, 8));
            sum32_128 = _mm_add_epi32(sum32_128, _mm_srli_si128(sum32_128, 4));
            
            sum += static_cast<uint32_t>(_mm_cvtsi128_si32(sum32_128));
            
            i += 24;
        }
#endif

        // Scalar fallback for remaining bytes
        while (i + 3 <= size) {
            const uint32_t b0 = data[i];
            const uint32_t b1 = data[i + 1];
            const uint32_t b2 = data[i + 2];
            
            const uint32_t a = b0 | ((b1 & 0x0Fu) << 8u);
            const uint32_t b = (b1 >> 4u) | (b2 << 4u);
            
            if (a > threshold) sum += a;
            if (b > threshold) sum += b;
            
            i += 3;
        }
    }
    
    return sum;
}