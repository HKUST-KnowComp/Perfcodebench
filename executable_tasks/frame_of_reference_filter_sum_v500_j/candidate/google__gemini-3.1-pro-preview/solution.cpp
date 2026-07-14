#include "interface.h"
#include <vector>
#include <cstdint>
#include <immintrin.h>
#include <algorithm>

uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int iters) {
    
    if (high < low) return 0;

    // Pre-calculate delta range [d_low, d_high]
    // base + delta >= low  => delta >= low - base
    // base + delta <= high => delta <= high - base
    int64_t d_low_64 = static_cast<int64_t>(low) - static_cast<int64_t>(base);
    int64_t d_high_64 = static_cast<int64_t>(high) - static_cast<int64_t>(base);

    if (d_high_64 < 0 || d_low_64 > 65535) return 0;

    uint16_t d_low = static_cast<uint16_t>(std::max<int64_t>(0, d_low_64));
    uint16_t d_high = static_cast<uint16_t>(std::min<int64_t>(65535, d_high_64));
    uint16_t d_range = d_high - d_low;

    const uint16_t* data = deltas.data();
    size_t n = deltas.size();
    uint64_t final_sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_delta_sum = 0;
        uint64_t current_count = 0;

        size_t i = 0;
        __m256i v_d_low = _mm256_set1_epi16(d_low);
        __m256i v_d_range = _mm256_set1_epi16(d_range);
        __m256i v_zero = _mm256_setzero_si256();
        __m256i v_delta_sum_acc = _mm256_setzero_si256();
        __m256i v_count_acc = _mm256_setzero_si256();
        __m256i v_one = _mm256_set1_epi16(1);

        for (; i + 15 < n; i += 16) {
            __m256i v_delta = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data + i));
            
            // Range check: (uint16_t)(delta - d_low) <= d_range
            __m256i v_sub = _mm256_sub_epi16(v_delta, v_d_low);
            __m256i v_mask = _mm256_cmpeq_epi16(_mm256_min_epu16(v_sub, v_d_range), v_sub);

            // Accumulate deltas and counts
            __m256i v_filtered_delta = _mm256_and_si256(v_mask, v_delta);
            __m256i v_filtered_count = _mm256_and_si256(v_mask, v_one);

            v_delta_sum_acc = _mm256_add_epi16(v_delta_sum_acc, v_filtered_delta);
            v_count_acc = _mm256_add_epi16(v_count_acc, v_filtered_count);

            // Periodically drain 16-bit accumulators to prevent overflow (max 65535 / 16 ~ 4096 iterations)
            if ((i & 0x7FFF) == 0x7FF0) {
                __m256i low_bits = _mm256_unpacklo_epi16(v_delta_sum_acc, v_zero);
                __m256i high_bits = _mm256_unpackhi_epi16(v_delta_sum_acc, v_zero);
                uint32_t tmp_deltas[8];
                _mm256_storeu_si256(reinterpret_cast<__m256i*>(tmp_deltas), _mm256_add_epi32(low_bits, high_bits));
                for(int k=0; k<8; ++k) current_delta_sum += tmp_deltas[k];

                low_bits = _mm256_unpacklo_epi16(v_count_acc, v_zero);
                high_bits = _mm256_unpackhi_epi16(v_count_acc, v_zero);
                _mm256_storeu_si256(reinterpret_cast<__m256i*>(tmp_deltas), _mm256_add_epi32(low_bits, high_bits));
                for(int k=0; k<8; ++k) current_count += tmp_deltas[k];

                v_delta_sum_acc = _mm256_setzero_si256();
                v_count_acc = _mm256_setzero_si256();
            }
        }

        // Final drain for SIMD accumulators
        uint16_t tmp_d[16], tmp_c[16];
        _mm256_storeu_si256(reinterpret_cast<__m256i*>(tmp_d), v_delta_sum_acc);
        _mm256_storeu_si256(reinterpret_cast<__m256i*>(tmp_c), v_count_acc);
        for (int k = 0; k < 16; ++k) {
            current_delta_sum += tmp_d[k];
            current_count += tmp_c[k];
        }

        // Scalar tail
        for (; i < n; ++i) {
            uint16_t d = data[i];
            if (static_cast<uint16_t>(d - d_low) <= d_range) {
                current_delta_sum += d;
                current_count++;
            }
        }

        final_sum = current_count * static_cast<uint64_t>(base) + current_delta_sum;
    }

    return final_sum;
}