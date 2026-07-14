#include "interface.h"
#include <cstdint>
#include <immintrin.h>

static inline uint64_t sum_all_avx2(const uint32_t* __restrict__ data, size_t n) {
    __m256i vsum = _mm256_setzero_si256();
    size_t i = 0;
    for (; i + 8 <= n; i += 8) {
        __m256i v = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data + i));
        __m256i lo = _mm256_unpacklo_epi32(v, _mm256_setzero_si256());
        __m256i hi = _mm256_unpackhi_epi32(v, _mm256_setzero_si256());
        vsum = _mm256_add_epi64(vsum, lo);
        vsum = _mm256_add_epi64(vsum, hi);
    }
    uint64_t tmp[4];
    _mm256_storeu_si256(reinterpret_cast<__m256i*>(tmp), vsum);
    uint64_t s = tmp[0] + tmp[1] + tmp[2] + tmp[3];
    for (; i < n; ++i) s += data[i];
    return s;
}

static inline uint64_t sum_filtered_avx2(const uint32_t* __restrict__ data, size_t n, uint32_t low, uint32_t high) {
    __m256i vlow = _mm256_set1_epi32(static_cast<int32_t>(low));
    __m256i vhigh = _mm256_set1_epi32(static_cast<int32_t>(high));
    __m256i vsum = _mm256_setzero_si256();
    __m256i vzero = _mm256_setzero_si256();
    size_t i = 0;
    for (; i + 8 <= n; i += 8) {
        __m256i v = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data + i));
        // unsigned compare: v >= low  <=>  max_u(v, low) == v
        __m256i cmp_ge = _mm256_cmpeq_epi32(_mm256_max_epu32(v, vlow), v);
        // v <= high <=> min_u(v, high) == v
        __m256i cmp_le = _mm256_cmpeq_epi32(_mm256_min_epu32(v, vhigh), v);
        __m256i mask = _mm256_and_si256(cmp_ge, cmp_le);
        __m256i masked = _mm256_and_si256(v, mask);
        __m256i lo = _mm256_unpacklo_epi32(masked, vzero);
        __m256i hi = _mm256_unpackhi_epi32(masked, vzero);
        vsum = _mm256_add_epi64(vsum, lo);
        vsum = _mm256_add_epi64(vsum, hi);
    }
    uint64_t tmp[4];
    _mm256_storeu_si256(reinterpret_cast<__m256i*>(tmp), vsum);
    uint64_t s = tmp[0] + tmp[1] + tmp[2] + tmp[3];
    for (; i < n; ++i) {
        uint32_t v = data[i];
        if (v >= low && v <= high) s += v;
    }
    return s;
}

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
    const uint32_t* __restrict__ vptr = values.data();
    const uint32_t* __restrict__ minptr = mins.data();
    const uint32_t* __restrict__ maxptr = maxs.data();

    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        size_t offset = 0;
        for (size_t b = 0; b < num_blocks; ++b) {
            uint32_t bmin = minptr[b];
            uint32_t bmax = maxptr[b];
            size_t end = offset + block_size;
            if (end > n) end = n;
            size_t len = end - offset;

            if (bmax < low || bmin > high) {
                // skip
            } else if (bmin >= low && bmax <= high) {
                sum += sum_all_avx2(vptr + offset, len);
            } else {
                sum += sum_filtered_avx2(vptr + offset, len, low, high);
            }
            offset = end;
        }
        // handle leftover if values extend past metadata coverage
        if (offset < n) {
            sum += sum_filtered_avx2(vptr + offset, n - offset, low, high);
        }
    }
    return sum;
}
