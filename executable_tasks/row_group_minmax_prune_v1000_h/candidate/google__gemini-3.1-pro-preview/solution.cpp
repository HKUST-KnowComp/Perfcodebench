#include "interface.h"
#include <immintrin.h>
#include <cstdint>
#include <vector>

uint64_t row_group_range_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& mins,
    const std::vector<uint32_t>& maxs,
    uint32_t low,
    uint32_t high,
    uint32_t block_size,
    int iters) {

    size_t n = values.size();
    size_t num_blocks = mins.size();
    uint64_t total_sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        for (size_t b = 0; b < num_blocks; ++b) {
            uint32_t b_min = mins[b];
            uint32_t b_max = maxs[b];

            // Case 1: Entire block is outside the range [low, high]
            if (b_min > high || b_max < low) {
                continue;
            }

            size_t start = b * block_size;
            size_t end = (b + 1) * block_size;
            if (end > n) end = n;

            // Case 2: Entire block is within the range [low, high]
            if (b_min >= low && b_max <= high) {
                __m256i sum_v = _mm256_setzero_si256();
                size_t i = start;
                for (; i + 7 < end; i += 8) {
                    __m256i v = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&values[i]));
                    __m256i v_lo = _mm256_unpacklo_epi32(v, _mm256_setzero_si256());
                    __m256i v_hi = _mm256_unpackhi_epi32(v, _mm256_setzero_si256());
                    sum_v = _mm256_add_epi64(sum_v, v_lo);
                    sum_v = _mm256_add_epi64(sum_v, v_hi);
                }
                uint64_t temp[4];
                _mm256_storeu_si256(reinterpret_cast<__m256i*>(temp), sum_v);
                current_sum += temp[0] + temp[1] + temp[2] + temp[3];
                for (; i < end; ++i) {
                    current_sum += values[i];
                }
            } 
            // Case 3: Partial overlap - filter elements
            else {
                __m256i sum_v = _mm256_setzero_si256();
                __m256i low_v = _mm256_set1_epi32(low - 1);
                __m256i high_v = _mm256_set1_epi32(high);
                size_t i = start;
                for (; i + 7 < end; i += 8) {
                    __m256i v = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&values[i]));
                    // mask = (v > low-1) AND (high >= v) => (v > low-1) AND NOT (v > high)
                    __m256i gt_low = _mm256_cmpgt_epi32(v, low_v);
                    __m256i gt_high = _mm256_cmpgt_epi32(v, high_v);
                    __m256i mask = _mm256_andnot_si256(gt_high, gt_low);
                    
                    __m256i filtered = _mm256_and_si256(v, mask);
                    __m256i v_lo = _mm256_unpacklo_epi32(filtered, _mm256_setzero_si256());
                    __m256i v_hi = _mm256_unpackhi_epi32(filtered, _mm256_setzero_si256());
                    sum_v = _mm256_add_epi64(sum_v, v_lo);
                    sum_v = _mm256_add_epi64(sum_v, v_hi);
                }
                uint64_t temp[4];
                _mm256_storeu_si256(reinterpret_cast<__m256i*>(temp), sum_v);
                current_sum += temp[0] + temp[1] + temp[2] + temp[3];
                for (; i < end; ++i) {
                    uint32_t val = values[i];
                    if (val >= low && val <= high) {
                        current_sum += val;
                    }
                }
            }
        }
        total_sum = current_sum;
    }

    return total_sum;
}