#include "interface.h"
#include <immintrin.h>
#include <cstdint>
#include <vector>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    const size_t n = packed.size();
    const size_t num_triplets = n / 3;
    const uint8_t* data = packed.data();
    uint64_t total_sum = 0;

    // Pre-calculate SIMD constants
    const __m256i shuffle_mask = _mm256_setr_epi8(
        0, 1, 1, 2, 3, 4, 4, 5, 6, 7, 7, 8, // Lane 0: 8 integers from 12 bytes
        -1, -1, -1, -1,                     // Padding
        0, 1, 1, 2, 3, 4, 4, 5, 6, 7, 7, 8, // Lane 1: 8 integers from 12 bytes
        -1, -1, -1, -1                      // Padding
    );
    const __m256i mask_even = _mm256_set1_epi16(0x0FFF);
    const __m256i thresh_vec = _mm256_set1_epi16(static_cast<int16_t>(threshold));
    const __m256i ones = _mm256_set1_epi16(1);

    for (int iter = 0; iter < iters; ++iter) {
        __m256i acc_64 = _mm256_setzero_si256();
        size_t i = 0;

        // Process 24 bytes (16 integers) at a time using AVX2
        // We need 24 bytes to produce 16 integers. 
        // We load 32 bytes but only use 12 bytes from each 16-byte lane.
        if (num_triplets >= 8) {
            for (; i + 24 <= n; i += 24) {
                // Load 32 bytes. Lane 0: [0..15], Lane 1: [16..31]
                // We want Lane 0 to use [0..11] and Lane 1 to use [12..23]
                // To align Lane 1, we use a permute or load carefully.
                __m256i raw = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data + i));
                
                // Fix Lane 1: it currently has [16..31], we want [12..27]
                // Use _mm256_permutevar8x32_epi32 to shift the window for the second lane
                // Specifically, we want bytes [0..15] in lane 0 and [12..27] in lane 1.
                // This is complex; simpler: load two 128-bit parts.
                __m128i low_lane = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data + i));
                __m128i high_lane = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data + i + 12));
                __m256i combined = _mm256_set_m128i(high_lane, low_lane);

                __m256i shuffled = _mm256_shuffle_epi8(combined, shuffle_mask);

                // Extract 12-bit values
                // Even indices (0, 2, ...): (val & 0x0FFF)
                // Odd indices (1, 3, ...): (val >> 4)
                __m256i even_vals = _mm256_and_si256(shuffled, mask_even);
                __m256i odd_vals = _mm256_srli_epi16(shuffled, 4);

                // Blend even and odd to get the 16 integers in one register
                // Pattern: E O E O E O E O ...
                __m256i vals = _mm256_blend_epi16(even_vals, odd_vals, 0xAA);

                // Filter: vals > threshold
                __m256i mask = _mm256_cmpgt_epi16(vals, thresh_vec);
                __m256i filtered = _mm256_and_si256(vals, mask);

                // Sum filtered values: madd converts 16-bit to 32-bit sums
                __m256i sums_32 = _mm256_madd_epi16(filtered, ones);
                
                // Accumulate 32-bit sums into 64-bit to avoid overflow
                acc_64 = _mm256_add_epi64(acc_64, _mm256_unpacklo_epi32(sums_32, _mm256_setzero_si256()));
                acc_64 = _mm256_add_epi64(acc_64, _mm256_unpackhi_epi32(sums_32, _mm256_setzero_si256()));
            }
        }

        // Horizontal sum of acc_64
        uint64_t current_sum = 0;
        alignas(32) uint64_t temp_sum[4];
        _mm256_store_si256(reinterpret_cast<__m256i*>(temp_sum), acc_64);
        current_sum = temp_sum[0] + temp_sum[1] + temp_sum[2] + temp_sum[3];

        // Remainder scalar processing
        for (; i + 2 < n; i += 3) {
            uint32_t a = static_cast<uint32_t>(data[i]) | (static_cast<uint32_t>(data[i + 1] & 0x0f) << 8);
            uint32_t b = (static_cast<uint32_t>(data[i + 1] >> 4)) | (static_cast<uint32_t>(data[i + 2]) << 4);
            if (a > threshold) current_sum += a;
            if (b > threshold) current_sum += b;
        }
        total_sum = current_sum;
    }

    return total_sum;
}
