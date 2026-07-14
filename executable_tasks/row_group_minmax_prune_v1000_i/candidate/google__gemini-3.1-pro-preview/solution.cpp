#include "interface.h"
#include <immintrin.h>
#include <cstdint>
#include <vector>

static inline uint64_t sum_block_all(const uint32_t* data, uint32_t size) {
    uint64_t total = 0;
    uint32_t i = 0;
    __m256i sum_vec = _mm256_setzero_si256();

    for (; i + 7 < size; i += 8) {
        __m256i v = _mm256_loadu_si256((const __m256i*)(data + i));
        __m256i low = _mm256_unpacklo_epi32(v, _mm256_setzero_si256());
        __m256i high = _mm256_unpackhi_epi32(v, _mm256_setzero_si256());
        sum_vec = _mm256_add_epi64(sum_vec, low);
        sum_vec = _mm256_add_epi64(sum_vec, high);
    }
    
    uint64_t sums[4];
    _mm256_storeu_si256((__m256i*)sums, sum_vec);
    total = sums[0] + sums[1] + sums[2] + sums[3];

    for (; i < size; ++i) {
        total += data[i];
    }
    return total;
}

static inline uint64_t sum_block_partial(const uint32_t* data, uint32_t size, uint32_t low, uint32_t high) {
    uint64_t total = 0;
    uint32_t i = 0;
    __m256i sum_vec = _mm256_setzero_si256();
    __m256i v_low = _mm256_set1_epi32(low);
    __m256i v_high = _mm256_set1_epi32(high);

    for (; i + 7 < size; i += 8) {
        __m256i v = _mm256_loadu_si256((const __m256i*)(data + i));
        // Unsigned comparison: (v >= low && v <= high)
        // Using min/max for unsigned comparison simulation
        __m256i clamped = _mm256_max_epu32(v, v_low);
        clamped = _mm256_min_epu32(clamped, v_high);
        __m256i mask = _mm256_cmpeq_epi32(clamped, v);

        __m256i filtered = _mm256_blendv_epi8(_mm256_setzero_si256(), v, mask);
        
        __m256i low_64 = _mm256_unpacklo_epi32(filtered, _mm256_setzero_si256());
        __m256i high_64 = _mm256_unpackhi_epi32(filtered, _mm256_setzero_si256());
        sum_vec = _mm256_add_epi64(sum_vec, low_64);
        sum_vec = _mm256_add_epi64(sum_vec, high_64);
    }

    uint64_t sums[4];
    _mm256_storeu_si256((__m256i*)sums, sum_vec);
    total = sums[0] + sums[1] + sums[2] + sums[3];

    for (; i < size; ++i) {
        if (data[i] >= low && data[i] <= high) {
            total += data[i];
        }
    }
    return total;
}

uint64_t row_group_range_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& mins,
    const std::vector<uint32_t>& maxs,
    uint32_t low,
    uint32_t high,
    uint32_t block_size,
    int iters) {
    
    uint64_t final_sum = 0;
    size_t num_values = values.size();
    size_t num_blocks = mins.size();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_iter_sum = 0;
        for (size_t b = 0; b < num_blocks; ++b) {
            uint32_t b_min = mins[b];
            uint32_t b_max = maxs[b];

            if (b_min > high || b_max < low) {
                continue;
            }

            size_t start = b * block_size;
            size_t end = (b + 1) * block_size;
            if (end > num_values) end = num_values;
            uint32_t current_block_size = (uint32_t)(end - start);

            if (b_min >= low && b_max <= high) {
                current_iter_sum += sum_block_all(&values[start], current_block_size);
            } else {
                current_iter_sum += sum_block_partial(&values[start], current_block_size, low, high);
            }
        }
        final_sum = current_iter_sum;
    }

    return final_sum;
}