#include "interface.h"

#include <cstdint>
#include <cstddef>

#if defined(__AVX2__)
#include <immintrin.h>
#endif

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    const uint8_t* __restrict data = packed.data();
    const std::size_t size = packed.size();
    const std::size_t num_triplets = size / 3u;
    uint64_t sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        std::size_t i = 0;

#if defined(__AVX2__)
        // Process 8 triplets (24 bytes) at a time -> 16 values
        const std::size_t simd_triplets = (num_triplets / 8u) * 8u;
        const __m256i thresh_vec = _mm256_set1_epi16(static_cast<int16_t>(threshold));
        __m256i sum_lo = _mm256_setzero_si256();
        __m256i sum_hi = _mm256_setzero_si256();

        for (; i < simd_triplets; i += 8u) {
            const uint8_t* p = data + i * 3u;

            // Load 24 bytes (we load 32 but only use 24)
            __m256i raw = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(p));
            // Extract lower 128 bits for first 4 triplets
            __m128i lo128 = _mm256_castsi256_si128(raw);
            // For bytes 12-23, we need to load separately to avoid issues
            __m128i hi128 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p + 12));

            // Decode 4 triplets from lo128 (bytes 0-11) -> 8 values
            // Triplet layout: [b0, b1, b2] -> val0 = b0 | ((b1 & 0x0F) << 8), val1 = (b1 >> 4) | (b2 << 4)
            
            // Shuffle to arrange bytes for decoding
            // For each triplet at offset k*3: bytes are at k*3, k*3+1, k*3+2
            // val0 needs: byte[k*3], byte[k*3+1] (low nibble)
            // val1 needs: byte[k*3+1] (high nibble), byte[k*3+2]
            
            // Shuffle pattern to get pairs of bytes for each 12-bit value
            // Output 16-bit words: [b0, b1], [b1, b2], [b3, b4], [b4, b5], ...
            const __m128i shuf_lo_a = _mm_setr_epi8(
                0, 1, 1, 2, 3, 4, 4, 5, 6, 7, 7, 8, 9, 10, 10, 11
            );
            __m128i arranged_lo = _mm_shuffle_epi8(lo128, shuf_lo_a);
            __m128i arranged_hi = _mm_shuffle_epi8(hi128, shuf_lo_a);

            // Now we have 16-bit words, but we need to extract 12-bit values
            // Even positions (0,2,4,6): val = word & 0x0FFF
            // Odd positions (1,3,5,7): val = word >> 4
            
            // Mask for even positions
            const __m128i mask_0fff = _mm_set1_epi16(0x0FFF);
            
            // For even values: AND with 0x0FFF
            __m128i even_lo = _mm_and_si128(arranged_lo, mask_0fff);
            __m128i even_hi = _mm_and_si128(arranged_hi, mask_0fff);
            
            // For odd values: shift right by 4
            __m128i odd_lo = _mm_srli_epi16(arranged_lo, 4);
            __m128i odd_hi = _mm_srli_epi16(arranged_hi, 4);
            
            // Interleave even and odd to get correct order
            // even_lo has: e0, x, e1, x, e2, x, e3, x (where x is garbage in odd positions)
            // odd_lo has: o0, x, o1, x, o2, x, o3, x
            // We need: e0, o0, e1, o1, e2, o2, e3, o3
            
            // Actually the shuffle gave us pairs, so:
            // arranged_lo[0] = [b0,b1] -> even gives val0, odd gives val1
            // arranged_lo[1] = [b1,b2] -> even gives garbage, odd gives garbage (we used wrong bytes)
            
            // Let me redo the shuffle more carefully
            // For 4 triplets (12 bytes), we get 8 values
            // val0 from bytes 0,1: b0 | ((b1&0xF)<<8) = (b1<<8|b0) & 0xFFF
            // val1 from bytes 1,2: (b1>>4) | (b2<<4) = ((b2<<8|b1) >> 4) & 0xFFF
            // val2 from bytes 3,4: same pattern
            // val3 from bytes 4,5: same pattern
            // etc.
            
            // Shuffle to create 16-bit words:
            // For val0: [b0, b1] -> word & 0xFFF
            // For val1: [b1, b2] -> word >> 4
            // For val2: [b3, b4] -> word & 0xFFF
            // For val3: [b4, b5] -> word >> 4
            // etc.
            
            // The shuffle I have creates exactly this pattern!
            // Position 0: [b0,b1] for val0 (mask)
            // Position 1: [b1,b2] for val1 (shift)
            // Position 2: [b3,b4] for val2 (mask)
            // Position 3: [b4,b5] for val3 (shift)
            // etc.
            
            // So even indices need mask, odd indices need shift
            // Blend based on position
            const __m128i blend_mask = _mm_setr_epi16(
                static_cast<short>(0x0000), static_cast<short>(0xFFFF),
                static_cast<short>(0x0000), static_cast<short>(0xFFFF),
                static_cast<short>(0x0000), static_cast<short>(0xFFFF),
                static_cast<short>(0x0000), static_cast<short>(0xFFFF)
            );
            
            __m128i vals_lo = _mm_blendv_epi8(even_lo, odd_lo, blend_mask);
            __m128i vals_hi = _mm_blendv_epi8(even_hi, odd_hi, blend_mask);
            
            // Combine into 256-bit register
            __m256i vals = _mm256_set_m128i(vals_hi, vals_lo);
            
            // Compare: keep values > threshold
            // _mm256_cmpgt_epi16 does signed comparison, but our values are 0-4095 which fits in signed range
            __m256i gt_mask = _mm256_cmpgt_epi16(vals, thresh_vec);
            
            // Zero out values <= threshold
            __m256i filtered = _mm256_and_si256(vals, gt_mask);
            
            // Accumulate: extend 16-bit to 32-bit and add
            __m128i filtered_lo = _mm256_castsi256_si128(filtered);
            __m128i filtered_hi = _mm256_extracti128_si256(filtered, 1);
            
            // Zero-extend 16->32
            __m256i ext_lo = _mm256_cvtepu16_epi32(filtered_lo);
            __m256i ext_hi = _mm256_cvtepu16_epi32(filtered_hi);
            
            sum_lo = _mm256_add_epi32(sum_lo, ext_lo);
            sum_hi = _mm256_add_epi32(sum_hi, ext_hi);
        }
        
        // Horizontal sum of 32-bit accumulators
        __m256i total = _mm256_add_epi32(sum_lo, sum_hi);
        __m128i total_lo = _mm256_castsi256_si128(total);
        __m128i total_hi = _mm256_extracti128_si256(total, 1);
        __m128i sum128 = _mm_add_epi32(total_lo, total_hi);
        sum128 = _mm_add_epi32(sum128, _mm_srli_si128(sum128, 8));
        sum128 = _mm_add_epi32(sum128, _mm_srli_si128(sum128, 4));
        sum = static_cast<uint64_t>(static_cast<uint32_t>(_mm_cvtsi128_si32(sum128)));
#endif

        // Scalar tail
        for (; i < num_triplets; ++i) {
            const std::size_t base = i * 3u;
            const uint32_t b0 = data[base];
            const uint32_t b1 = data[base + 1u];
            const uint32_t b2 = data[base + 2u];
            
            const uint32_t a = b0 | ((b1 & 0x0Fu) << 8u);
            const uint32_t b = (b1 >> 4u) | (b2 << 4u);
            
            if (a > threshold) sum += a;
            if (b > threshold) sum += b;
        }
    }
    return sum;
}