#include "interface.h"
#include <immintrin.h>
#include <cstdint>

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
    const __m256i vlow  = _mm256_set1_epi32(static_cast<int32_t>(low));
    const __m256i vhigh = _mm256_set1_epi32(static_cast<int32_t>(high));
    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        size_t base = 0;
        for (size_t b = 0; b < num_blocks; ++b) {
            uint32_t block_min = mn[b];
            uint32_t block_max = mx[b];
            if (block_min > high || block_max < low) {
                base += block_size;
                continue;
            }
            const uint32_t* block = v + base;
            const uint32_t* end   = block + block_size;
            __m256i vacc = _mm256_setzero_si256();
            for (const uint32_t* p = block; p + 8 <= end; p += 8) {
                __m256i val = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(p));
                __m256i ge_low  = _mm256_cmpgt_epi32(val, vlow);
                __m256i eq_low  = _mm256_cmpeq_epi32(val, vlow);
                __m256i ok_low  = _mm256_or_si256(ge_low, eq_low);
                __m256i le_high = _mm256_cmpgt_epi32(vhigh, val);
                __m256i eq_high = _mm256_cmpeq_epi32(val, vhigh);
                __m256i ok_high = _mm256_or_si256(le_high, eq_high);
                __m256i mask    = _mm256_and_si256(ok_low, ok_high);
                __m256i contrib = _mm256_and_si256(mask, val);
                vacc = _mm256_add_epi64(
                    vacc,
                    _mm256_unpacklo_epi32(contrib, _mm256_setzero_si256())
                );
                vacc = _mm256_add_epi64(
                    vacc,
                    _mm256_unpackhi_epi32(contrib, _mm256_setzero_si256())
                );
            }
            alignas(32) uint64_t partial[4];
            _mm256_store_si256(reinterpret_cast<__m256i*>(partial), vacc);
            sum += partial[0] + partial[1] + partial[2] + partial[3];
            for (const uint32_t* p = block + (block_size & ~7U); p < end; ++p) {
                uint32_t x = *p;
                if (x >= low && x <= high) sum += x;
            }
            base += block_size;
        }
    }
    return sum;
}