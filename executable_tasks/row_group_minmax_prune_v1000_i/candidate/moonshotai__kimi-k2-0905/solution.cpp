#include "interface.h"
#include <immintrin.h>
#include <cstdint>
#include <cstring>

namespace {

// ---------- SIMD helpers ----------

inline uint64_t sum_u32_in_range_avx512(const uint32_t* ptr, size_t n,
                                         uint32_t low, uint32_t high) {
    __m512i vlow  = _mm512_set1_epi32(static_cast<int32_t>(low));
    __m512i vhigh = _mm512_set1_epi32(static_cast<int32_t>(high));
    __m512i acc   = _mm512_setzero_si512();
    size_t i = 0;
    for (; i + 16 <= n; i += 16) {
        __m512i v = _mm512_loadu_si512(reinterpret_cast<const __m512i*>(ptr + i));
        __mmask16 m = _mm512_and_epi32(
                        _mm512_cmpge_epu32_mask(v, vlow),
                        _mm512_cmple_epu32_mask(v, vhigh));
        __m512i msk = _mm512_maskz_set1_epi32(m, 1);
        __m512i add = _mm512_and_si512(v, msk);
        acc = _mm512_add_epi64(acc, _mm512_unpacklo_epi32(add, _mm512_setzero_si512()));
        acc = _mm512_add_epi64(acc, _mm512_unpackhi_epi32(add, _mm512_setzero_si512()));
    }
    __m256i lo = _mm512_extracti64x4_epi64(acc, 0);
    __m256i hi = _mm512_extracti64x4_epi64(acc, 1);
    lo = _mm256_add_epi64(lo, hi);
    uint64_t tmp[4];
    _mm256_storeu_si256(reinterpret_cast<__m256i*>(tmp), lo);
    uint64_t sum = tmp[0] + tmp[1] + tmp[2] + tmp[3];
    // scalar tail
    for (; i < n; ++i) {
        uint32_t v = ptr[i];
        if (v >= low && v <= high) sum += v;
    }
    return sum;
}

inline uint64_t sum_u32_in_range_avx2(const uint32_t* ptr, size_t n,
                                      uint32_t low, uint32_t high) {
    __m256i vlow  = _mm256_set1_epi32(static_cast<int32_t>(low));
    __m256i vhigh = _mm256_set1_epi32(static_cast<int32_t>(high));
    __m256i acc   = _mm256_setzero_si256();
    size_t i = 0;
    for (; i + 8 <= n; i += 8) {
        __m256i v = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(ptr + i));
        __m256i cmp_lo = _mm256_cmpgt_epi32(vlow, v);
        __m256i cmp_hi = _mm256_cmpgt_epi32(v, vhigh);
        __m256i m = _mm256_or_si256(cmp_lo, cmp_hi);  // 0xFFFFFFFF where out of range
        __m256i zero = _mm256_setzero_si256();
        __m256i add = _mm256_blendv_epi8(v, zero, m);
        // horizontal add to 64-bit accumulators
        __m256i t1 = _mm256_unpacklo_epi32(add, zero);
        __m256i t2 = _mm256_unpackhi_epi32(add, zero);
        acc = _mm256_add_epi64(acc, t1);
        acc = _mm256_add_epi64(acc, t2);
    }
    // horizontal sum of acc
    __m256i sh = _mm256_shuffle_epi32(acc, 0x0E);
    acc = _mm256_add_epi64(acc, sh);
    uint64_t tmp[4];
    _mm256_storeu_si256(reinterpret_cast<__m256i*>(tmp), acc);
    uint64_t sum = tmp[0] + tmp[1];
    // scalar tail
    for (; i < n; ++i) {
        uint32_t v = ptr[i];
        if (v >= low && v <= high) sum += v;
    }
    return sum;
}

inline uint64_t sum_u32_in_range_scalar(const uint32_t* ptr, size_t n,
                                        uint32_t low, uint32_t high) {
    uint64_t sum = 0;
    for (size_t i = 0; i < n; ++i) {
        uint32_t v = ptr[i];
        if (v >= low && v <= high) sum += v;
    }
    return sum;
}

inline uint64_t sum_u32_in_range(const uint32_t* ptr, size_t n,
                               uint32_t low, uint32_t high) {
#if defined(__AVX512F__)
    return sum_u32_in_range_avx512(ptr, n, low, high);
#elif defined(__AVX2__)
    return sum_u32_in_range_avx2(ptr, n, low, high);
#else
    return sum_u32_in_range_scalar(ptr, n, low, high);
#endif
}

} // anonymous namespace

uint64_t row_group_range_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& mins,
    const std::vector<uint32_t>& maxs,
    uint32_t low,
    uint32_t high,
    uint32_t block_size,
    int iters) {
    const size_t n = values.size();
    const size_t num_blocks = (n + block_size - 1) / block_size;
    uint64_t total = 0;
    for (int iter = 0; iter < iters; ++iter) {
        total = 0;
        for (size_t b = 0; b < num_blocks; ++b) {
            uint32_t bmin = mins[b];
            uint32_t bmax = maxs[b];
            if (bmax < low || bmin > high) continue; // prune block
            size_t start = b * block_size;
            size_t end   = start + block_size;
            if (end > n) end = n;
            total += sum_u32_in_range(values.data() + start, end - start, low, high);
        }
    }
    return total;
}