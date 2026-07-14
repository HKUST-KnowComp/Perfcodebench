#include "interface.h"
#include <immintrin.h>
#include <cstdint>
#include <vector>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    if (packed.empty()) return 0;

    const size_t n_bytes = packed.size();
    const size_t n_blocks = n_bytes / 24;
    const uint16_t* packed_ptr = reinterpret_cast<const uint16_t*>(packed.data());
    
    __m256i v_threshold = _mm256_set1_epi16(static_cast<short>(threshold));
    __m256i v_mask_even = _mm256_set1_epi16(0x0FFF);
    __m256i v_shuffle = _mm256_setr_epi8(
        0, 1, 1, 2, 3, 4, 4, 5, 6, 7, 7, 8, 9, 10, 10, 11,
        12, 13, 13, 14, 15, 16, 16, 17, 18, 19, 19, 20, 21, 22, 22, 23
    );

    uint64_t final_sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        __m256i v_sum_lo = _mm256_setzero_si256();
        __m256i v_sum_hi = _mm256_setzero_si256();
        
        size_t i = 0;
        for (; i < n_blocks; ++i) {
            // Load 24 bytes (using 32-byte load, safe if vector has padding or small enough)
            __m256i raw = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(packed.data() + i * 24));
            __m256i shuffled = _mm256_shuffle_epi8(raw, v_shuffle);

            // Even lanes: (p[0] | p[1] << 8) & 0x0FFF
            // Odd lanes: (p[1] | p[2] << 8) >> 4
            __m256i even_vals = _mm256_and_si256(shuffled, v_mask_even);
            __m256i odd_vals = _mm256_srli_epi16(shuffled, 4);

            // Blend even and odd lanes
            __m256i values = _mm256_blend_epi16(even_vals, odd_vals, 0xAA);

            // Filter: values > threshold
            // _mm256_cmpgt_epi16 works on signed shorts, but 12-bit values (0-4095) fit in signed positive range
            __m256i mask = _mm256_cmpgt_epi16(values, v_threshold);
            __m256i filtered = _mm256_and_si256(values, mask);

            // Accumulate into 64-bit sums to avoid overflow
            __m256i filtered_lo = _mm256_unpacklo_epi16(filtered, _mm256_setzero_si256());
            __m256i filtered_hi = _mm256_unpackhi_epi16(filtered, _mm256_setzero_si256());

            v_sum_lo = _mm256_add_epi64(v_sum_lo, _mm256_unpacklo_epi32(filtered_lo, _mm256_setzero_si256()));
            v_sum_lo = _mm256_add_epi64(v_sum_lo, _mm256_unpackhi_epi32(filtered_lo, _mm256_setzero_si256()));
            v_sum_hi = _mm256_add_epi64(v_sum_hi, _mm256_unpacklo_epi32(filtered_hi, _mm256_setzero_si256()));
            v_sum_hi = _mm256_add_epi64(v_sum_hi, _mm256_unpackhi_epi32(filtered_hi, _mm256_setzero_si256()));
        }

        uint64_t sums[4];
        _mm256_storeu_si256(reinterpret_cast<__m256i*>(sums), _mm256_add_epi64(v_sum_lo, v_sum_hi));
        final_sum = sums[0] + sums[1] + sums[2] + sums[3];

        // Scalar tail
        for (size_t j = n_blocks * 24; j + 2 < n_bytes; j += 3) {
            uint32_t a = static_cast<uint32_t>(packed[j]) | (static_cast<uint32_t>(packed[j + 1] & 0x0F) << 8);
            uint32_t b = static_cast<uint32_t>(packed[j + 1] >> 4) | (static_cast<uint32_t>(packed[j + 2]) << 4);
            if (a > threshold) final_sum += a;
            if (b > threshold) final_sum += b;
        }
    }

    return final_sum;
}