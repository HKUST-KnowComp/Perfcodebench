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

    if (values.empty() || iters <= 0) return 0;

    const uint32_t *v = values.data();
    const size_t n = values.size();
    const size_t groups = mins.size();

    // Pre-compute masks used in fine scan
    const __m256i vlow  = _mm256_set1_epi32(static_cast<int32_t>(low));
    const __m256i vhigh = _mm256_set1_epi32(static_cast<int32_t>(high));

    uint64_t total = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t sum = 0;

        // ---------- metadata pass ----------
        size_t idx = 0;
        for (size_t g = 0; g < groups; ++g) {
            const uint32_t mn = mins[g];
            const uint32_t mx = maxs[g];

            if (mn > high || mx < low) {
                idx += block_size;
                continue;
            }
            if (mn >= low && mx <= high) {
                // whole block in range
                const size_t end = idx + block_size;
                for (; idx + 7 < end; idx += 8) {
                    __m256i val = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(v + idx));
                    __m256i lo  = _mm256_unpacklo_epi32(val, _mm256_setzero_si256());
                    __m256i hi  = _mm256_unpackhi_epi32(val, _mm256_setzero_si256());
                    sum += static_cast<uint64_t>(_mm256_extract_epi64(lo, 0)) +
                           static_cast<uint64_t>(_mm256_extract_epi64(lo, 1)) +
                           static_cast<uint64_t>(_mm256_extract_epi64(lo, 2)) +
                           static_cast<uint64_t>(_mm256_extract_epi64(lo, 3)) +
                           static_cast<uint64_t>(_mm256_extract_epi64(hi, 0)) +
                           static_cast<uint64_t>(_mm256_extract_epi64(hi, 1)) +
                           static_cast<uint64_t>(_mm256_extract_epi64(hi, 2)) +
                           static_cast<uint64_t>(_mm256_extract_epi64(hi, 3));
                }
                for (; idx < end; ++idx) sum += v[idx];
            } else {
                // fine scan
                const size_t end = idx + block_size;
                for (; idx + 7 < end; idx += 8) {
                    __m256i val = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(v + idx));
                    __m256i mask = _mm256_and_si256(_mm256_cmpgt_epi32(val, _mm256_sub_epi32(vlow, _mm256_set1_epi32(1))),
                                                   _mm256_cmpgt_epi32(vhigh, _mm256_sub_epi32(val, _mm256_set1_epi32(1))));
                    const int m = _mm256_movemask_ps(_mm256_castsi256_ps(mask));
                    if (m == 0) continue;

                    uint32_t buf[8];
                    _mm256_storeu_si256((__m256i*)buf, val);
                    for (int k = 0; k < 8; ++k) {
                        uint32_t x = buf[k];
                        sum += (x >= low && x <= high) ? x : 0;
                    }
                }
                for (; idx < end; ++idx) {
                    uint32_t x = v[idx];
                    sum += (x >= low && x <= high) ? x : 0;
                }
            }
        }

        // tail elements beyond last complete block
        for (; idx < n; ++idx) {
            uint32_t x = v[idx];
            sum += (x >= low && x <= high) ? x : 0;
        }

        total = sum;
    }

    return total;
}