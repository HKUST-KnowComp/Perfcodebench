#include "interface.h"
#include <cstdint>
#include <vector>
#include <immintrin.h>

uint64_t row_group_range_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& mins,
    const std::vector<uint32_t>& maxs,
    uint32_t low,
    uint32_t high,
    uint32_t block_size,
    int iters)
{
    const uint32_t* v = values.data();
    const uint32_t* mn = mins.data();
    const uint32_t* mx = maxs.data();
    const size_t num_blocks = mins.size();
    const uint64_t low64  = low;
    const uint64_t high64 = high;

    uint64_t total = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t sum = 0;
        size_t base = 0;
        for (size_t b = 0; b < num_blocks; ++b) {
            const uint32_t block_min = mn[b];
            const uint32_t block_max = mx[b];
            if (block_max < low || block_min > high) {
                base += block_size;
                continue;
            }
            const uint32_t* block = v + base;
            const uint32_t* end   = block + block_size;

            const __m256i vlow  = _mm256_set1_epi32(static_cast<int32_t>(low));
            const __m256i vhigh = _mm256_set1_epi32(static_cast<int32_t>(high));

            const uint32_t* ptr = block;
            for (; ptr + 8 <= end; ptr += 8) {
                __m256i val = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(ptr));
                __m256i ge_low  = _mm256_cmpgt_epi32(val, _mm256_sub_epi32(vlow,  _mm256_set1_epi32(1)));
                __m256i le_high = _mm256_cmpgt_epi32(_mm256_add_epi32(vhigh, _mm256_set1_epi32(1)), val);
                __m256i ok      = _mm256_and_si256(ge_low, le_high);
                __m256i zero    = _mm256_setzero_si256();
                __m256i masked  = _mm256_blendv_epi8(zero, val, ok);
                __m256i lo      = _mm256_unpacklo_epi32(masked, zero);
                __m256i hi      = _mm256_unpackhi_epi32(masked, zero);
                __m256i sum64   = _mm256_add_epi64(lo, hi);
                alignas(32) uint64_t parts[4];
                _mm256_store_si256(reinterpret_cast<__m256i*>(parts), sum64);
                sum += parts[0] + parts[1] + parts[2] + parts[3];
            }
            for (; ptr < end; ++ptr) {
                uint32_t x = *ptr;
                if (x >= low && x <= high) sum += x;
            }
            base += block_size;
        }
        total = sum;
    }
    return total;
}