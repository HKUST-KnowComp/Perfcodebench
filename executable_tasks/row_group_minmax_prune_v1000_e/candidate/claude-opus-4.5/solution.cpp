#include "interface.h"
#include <cstdint>
#include <immintrin.h>

uint64_t row_group_range_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& mins,
    const std::vector<uint32_t>& maxs,
    uint32_t low,
    uint32_t high,
    uint32_t block_size,
    int iters) {

    const size_t n = values.size();
    const size_t num_blocks = mins.size();
    const uint32_t* __restrict vptr = values.data();
    const uint32_t* __restrict minptr = mins.data();
    const uint32_t* __restrict maxptr = maxs.data();

    uint64_t sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;

        for (size_t b = 0; b < num_blocks; ++b) {
            uint32_t block_min = minptr[b];
            uint32_t block_max = maxptr[b];

            // Prune: block entirely outside [low, high]
            if (block_max < low || block_min > high) {
                continue;
            }

            size_t start = b * block_size;
            size_t end = start + block_size;
            if (end > n) end = n;
            size_t count = end - start;

            const uint32_t* block_ptr = vptr + start;

            // Full include: block entirely inside [low, high]
            if (block_min >= low && block_max <= high) {
                size_t i = 0;
                __m256i acc0 = _mm256_setzero_si256();
                __m256i acc1 = _mm256_setzero_si256();

                // Process 16 elements per iteration (2x8)
                for (; i + 15 < count; i += 16) {
                    __m256i v0 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(block_ptr + i));
                    __m256i v1 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(block_ptr + i + 8));

                    // Unpack to 64-bit and accumulate
                    __m256i lo0 = _mm256_unpacklo_epi32(v0, _mm256_setzero_si256());
                    __m256i hi0 = _mm256_unpackhi_epi32(v0, _mm256_setzero_si256());
                    __m256i lo1 = _mm256_unpacklo_epi32(v1, _mm256_setzero_si256());
                    __m256i hi1 = _mm256_unpackhi_epi32(v1, _mm256_setzero_si256());

                    acc0 = _mm256_add_epi64(acc0, lo0);
                    acc0 = _mm256_add_epi64(acc0, hi0);
                    acc1 = _mm256_add_epi64(acc1, lo1);
                    acc1 = _mm256_add_epi64(acc1, hi1);
                }

                // Process remaining 8 elements
                for (; i + 7 < count; i += 8) {
                    __m256i v0 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(block_ptr + i));
                    __m256i lo0 = _mm256_unpacklo_epi32(v0, _mm256_setzero_si256());
                    __m256i hi0 = _mm256_unpackhi_epi32(v0, _mm256_setzero_si256());
                    acc0 = _mm256_add_epi64(acc0, lo0);
                    acc0 = _mm256_add_epi64(acc0, hi0);
                }

                // Horizontal sum
                acc0 = _mm256_add_epi64(acc0, acc1);
                __m128i lo128 = _mm256_castsi256_si128(acc0);
                __m128i hi128 = _mm256_extracti128_si256(acc0, 1);
                __m128i sum128 = _mm_add_epi64(lo128, hi128);
                uint64_t arr[2];
                _mm_storeu_si128(reinterpret_cast<__m128i*>(arr), sum128);
                sum += arr[0] + arr[1];

                // Scalar tail
                for (; i < count; ++i) {
                    sum += block_ptr[i];
                }
            } else {
                // Partial overlap: must filter each element
                size_t i = 0;
                __m256i vlow = _mm256_set1_epi32(static_cast<int32_t>(low));
                __m256i vhigh = _mm256_set1_epi32(static_cast<int32_t>(high));
                __m256i acc0 = _mm256_setzero_si256();
                __m256i acc1 = _mm256_setzero_si256();

                for (; i + 15 < count; i += 16) {
                    __m256i v0 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(block_ptr + i));
                    __m256i v1 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(block_ptr + i + 8));

                    // Unsigned comparison: v >= low  <=>  max(v, low) == v
                    // v <= high <=> min(v, high) == v
                    __m256i ge_low0 = _mm256_cmpeq_epi32(_mm256_max_epu32(v0, vlow), v0);
                    __m256i le_high0 = _mm256_cmpeq_epi32(_mm256_min_epu32(v0, vhigh), v0);
                    __m256i mask0 = _mm256_and_si256(ge_low0, le_high0);

                    __m256i ge_low1 = _mm256_cmpeq_epi32(_mm256_max_epu32(v1, vlow), v1);
                    __m256i le_high1 = _mm256_cmpeq_epi32(_mm256_min_epu32(v1, vhigh), v1);
                    __m256i mask1 = _mm256_and_si256(ge_low1, le_high1);

                    __m256i masked0 = _mm256_and_si256(v0, mask0);
                    __m256i masked1 = _mm256_and_si256(v1, mask1);

                    __m256i lo0 = _mm256_unpacklo_epi32(masked0, _mm256_setzero_si256());
                    __m256i hi0 = _mm256_unpackhi_epi32(masked0, _mm256_setzero_si256());
                    __m256i lo1 = _mm256_unpacklo_epi32(masked1, _mm256_setzero_si256());
                    __m256i hi1 = _mm256_unpackhi_epi32(masked1, _mm256_setzero_si256());

                    acc0 = _mm256_add_epi64(acc0, lo0);
                    acc0 = _mm256_add_epi64(acc0, hi0);
                    acc1 = _mm256_add_epi64(acc1, lo1);
                    acc1 = _mm256_add_epi64(acc1, hi1);
                }

                for (; i + 7 < count; i += 8) {
                    __m256i v0 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(block_ptr + i));
                    __m256i ge_low0 = _mm256_cmpeq_epi32(_mm256_max_epu32(v0, vlow), v0);
                    __m256i le_high0 = _mm256_cmpeq_epi32(_mm256_min_epu32(v0, vhigh), v0);
                    __m256i mask0 = _mm256_and_si256(ge_low0, le_high0);
                    __m256i masked0 = _mm256_and_si256(v0, mask0);
                    __m256i lo0 = _mm256_unpacklo_epi32(masked0, _mm256_setzero_si256());
                    __m256i hi0 = _mm256_unpackhi_epi32(masked0, _mm256_setzero_si256());
                    acc0 = _mm256_add_epi64(acc0, lo0);
                    acc0 = _mm256_add_epi64(acc0, hi0);
                }

                acc0 = _mm256_add_epi64(acc0, acc1);
                __m128i lo128 = _mm256_castsi256_si128(acc0);
                __m128i hi128 = _mm256_extracti128_si256(acc0, 1);
                __m128i sum128 = _mm_add_epi64(lo128, hi128);
                uint64_t arr[2];
                _mm_storeu_si128(reinterpret_cast<__m128i*>(arr), sum128);
                sum += arr[0] + arr[1];

                // Scalar tail
                for (; i < count; ++i) {
                    uint32_t v = block_ptr[i];
                    if (v >= low && v <= high) {
                        sum += v;
                    }
                }
            }
        }
    }

    return sum;
}
