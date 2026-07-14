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
    int iters)
{
    const uint32_t n_blocks = mins.size();
    const __m256i vlow  = _mm256_set1_epi32(static_cast<int32_t>(low));
    const __m256i vhigh = _mm256_set1_epi32(static_cast<int32_t>(high));
    uint64_t total = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t sum = 0;
        for (uint32_t b = 0; b < n_blocks; ++b) {
            if (maxs[b] < low || mins[b] > high) continue;
            const uint32_t base = b * block_size;
            const uint32_t limit = base + block_size;
            const uint32_t* ptr = values.data() + base;
            uint32_t i = 0;

            // SIMD main loop: 8 uint32_t per iteration
            for (; i + 8 <= block_size; i += 8) {
                __m256i v = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(ptr + i));
                __m256i ge_low  = _mm256_cmpgt_epi32(v, _mm256_sub_epi32(vlow, _mm256_set1_epi32(1)));
                __m256i le_high = _mm256_cmpgt_epi32(_mm256_add_epi32(vhigh, _mm256_set1_epi32(1)), v);
                __m256i mask    = _mm256_and_si256(ge_low, le_high);
                __m256i zero    = _mm256_setzero_si256();
                __m256i masked  = _mm256_blendv_epi8(zero, v, mask);
                // horizontal add of 8 uint32_t into 64-bit
                __m256i lo64 = _mm256_unpacklo_epi32(masked, zero);
                __m256i hi64 = _mm256_unpackhi_epi32(masked, zero);
                __m256i sum64 = _mm256_add_epi64(lo64, hi64);
                sum64 = _mm256_add_epi64(sum64, _mm256_srli_si256(sum64, 8));
                sum64 = _mm256_add_epi64(sum64, _mm256_srli_si256(sum64, 4));
                sum += static_cast<uint64_t>(_mm256_extract_epi64(sum64, 0)) +
                       static_cast<uint64_t>(_mm256_extract_epi64(sum64, 1)) +
                       static_cast<uint64_t>(_mm256_extract_epi64(sum64, 2)) +
                       static_cast<uint64_t>(_mm256_extract_epi64(sum64, 3));
            }

            // scalar tail
            for (; i < block_size && base + i < values.size(); ++i) {
                uint32_t val = ptr[i];
                if (val >= low && val <= high) sum += val;
            }
        }
        total = sum;
    }
    return total;
}