#include "interface.h"

#include <cstdint>
#include <cstddef>

#if defined(__AVX2__)
#include <immintrin.h>
#endif

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    const uint8_t* __restrict data = packed.data();
    const std::size_t size = packed.size();
    uint64_t sum = 0;

#if defined(__AVX2__)
    // Process 24 bytes at a time -> 16 x 12-bit values
    // We'll use a 32-byte load but only consume 24 bytes per iteration
    const std::size_t chunk24 = 24;
    const std::size_t num_chunks = size / chunk24;
    const std::size_t simd_end = num_chunks * chunk24;

    // Shuffle masks to unpack 24 bytes into 16 x 16-bit values
    // Input bytes: b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 b23
    // Values: v0 = b0 | (b1 & 0x0F) << 8
    //         v1 = (b1 >> 4) | b2 << 4
    //         v2 = b3 | (b4 & 0x0F) << 8
    //         ...
    // For even indices (0,2,4,...): low byte at 3*i, high nibble at 3*i+1 (masked)
    // For odd indices (1,3,5,...): low nibble at 3*i-2 (shifted), high byte at 3*i-1 (shifted)

    // Shuffle for low 8 values (bytes 0-11)
    const __m256i shuf_lo = _mm256_setr_epi8(
        0, 1, 1, 2, 3, 4, 4, 5, 6, 7, 7, 8, 9, 10, 10, 11,
        0, 1, 1, 2, 3, 4, 4, 5, 6, 7, 7, 8, 9, 10, 10, 11
    );

    const __m256i mask_even = _mm256_set1_epi16(0x0FFF);
    const __m256i thresh_vec = _mm256_set1_epi16(static_cast<int16_t>(threshold));

    for (int iter = 0; iter < iters; ++iter) {
        __m256i acc_lo = _mm256_setzero_si256();
        __m256i acc_hi = _mm256_setzero_si256();

        std::size_t i = 0;
        for (; i < simd_end; i += chunk24) {
            // Load 24 bytes (we load 32 but only use 24)
            // Process first 12 bytes -> 8 values
            __m128i raw0 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data + i));
            // Process second 12 bytes -> 8 values  
            __m128i raw1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data + i + 12));

            // Combine into 256-bit register
            __m256i raw = _mm256_set_m128i(raw1, raw0);

            // Shuffle to get pairs of bytes for each 12-bit value
            __m256i shuffled = _mm256_shuffle_epi8(raw, shuf_lo);

            // For even positions: value = (hi << 8 | lo) & 0x0FFF
            // For odd positions: value = (hi << 8 | lo) >> 4
            // After shuffle, even positions have correct bytes, odd positions need shift

            // Extract even and odd 16-bit values
            __m256i even_vals = _mm256_and_si256(shuffled, mask_even);
            __m256i odd_vals = _mm256_srli_epi16(shuffled, 4);
            odd_vals = _mm256_and_si256(odd_vals, mask_even);

            // Blend even and odd: even at positions 0,2,4,6; odd at 1,3,5,7
            // blend16 uses immediate mask where bit i selects from b if 1, a if 0
            __m256i vals = _mm256_blend_epi16(even_vals, odd_vals, 0xAA); // 0xAA = 10101010b

            // Compare vals > threshold (unsigned comparison via saturating subtract)
            // If val > threshold, then val - threshold - 1 won't saturate to 0
            // Use signed comparison after biasing
            __m256i cmp = _mm256_cmpgt_epi16(vals, thresh_vec);

            // Zero out values <= threshold
            __m256i filtered = _mm256_and_si256(vals, cmp);

            // Accumulate: extend 16-bit to 32-bit and add
            __m256i lo_32 = _mm256_unpacklo_epi16(filtered, _mm256_setzero_si256());
            __m256i hi_32 = _mm256_unpackhi_epi16(filtered, _mm256_setzero_si256());

            acc_lo = _mm256_add_epi32(acc_lo, lo_32);
            acc_hi = _mm256_add_epi32(acc_hi, hi_32);
        }

        // Horizontal sum of accumulators
        __m256i acc_sum = _mm256_add_epi32(acc_lo, acc_hi);
        // Extend to 64-bit and sum
        __m256i lo_64 = _mm256_unpacklo_epi32(acc_sum, _mm256_setzero_si256());
        __m256i hi_64 = _mm256_unpackhi_epi32(acc_sum, _mm256_setzero_si256());
        __m256i sum_64 = _mm256_add_epi64(lo_64, hi_64);

        // Extract and sum lanes
        __m128i sum_128_lo = _mm256_castsi256_si128(sum_64);
        __m128i sum_128_hi = _mm256_extracti128_si256(sum_64, 1);
        __m128i sum_128 = _mm_add_epi64(sum_128_lo, sum_128_hi);
        sum = static_cast<uint64_t>(_mm_extract_epi64(sum_128, 0)) +
              static_cast<uint64_t>(_mm_extract_epi64(sum_128, 1));

        // Handle remaining bytes with scalar code
        for (; i + 2 < size; i += 3) {
            uint32_t a = static_cast<uint32_t>(data[i]) |
                        (static_cast<uint32_t>(data[i + 1] & 0x0F) << 8);
            uint32_t b = (static_cast<uint32_t>(data[i + 1] >> 4)) |
                        (static_cast<uint32_t>(data[i + 2]) << 4);
            if (a > threshold) sum += a;
            if (b > threshold) sum += b;
        }
    }
#else
    // Scalar fallback: fused decode + filter + sum (no intermediate storage)
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        for (std::size_t i = 0; i + 2 < size; i += 3) {
            uint32_t a = static_cast<uint32_t>(data[i]) |
                        (static_cast<uint32_t>(data[i + 1] & 0x0F) << 8);
            uint32_t b = (static_cast<uint32_t>(data[i + 1] >> 4)) |
                        (static_cast<uint32_t>(data[i + 2]) << 4);
            if (a > threshold) sum += a;
            if (b > threshold) sum += b;
        }
    }
#endif

    return sum;
}