#include "interface.h"

#include <cstdint>
#include <vector>
#include <immintrin.h>

namespace {

inline bool cpu_has_avx2() noexcept {
    static const bool has = []() noexcept {
        unsigned int eax, ebx, ecx, edx;
        if (__get_cpuid_count(7, 0, &eax, &ebx, &ecx, &edx) == 0) return false;
        return (ebx & (1u << 5)) != 0;  // AVX2 bit
    }();
    return has;
}

uint64_t filter_sum_avx2(
    const uint32_t* __restrict keys,
    const uint32_t* __restrict values,
    const uint8_t* __restrict flags,
    uint32_t key_threshold,
    std::size_t n) noexcept
{
    const __m256i vk_thresh = _mm256_set1_epi32(static_cast<int32_t>(key_threshold));
    const __m256i vk_mask   = _mm256_set1_epi32(3);
    __m256i vacc   = _mm256_setzero_si256();

    std::size_t i = 0;
    for (; i + 8 <= n; i += 8) {
        const __m256i vk   = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(keys + i));
        const __m256i vv   = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(values + i));

        // flags[i] != 0 test (8 x uint8 -> 8 x 32-bit mask)
        const __m128i fl8  = _mm_loadl_epi64(reinterpret_cast<const __m128i*>(flags + i));
        const __m256i fl32 = _mm256_cvtepu8_epi32(fl8);
        const __m256i fmsk = _mm256_cmpgt_epi32(fl32, _mm256_setzero_si256());

        // keys[i] < key_threshold
        const __m256i kmask = _mm256_cmpgt_epi32(vk_thresh, vk);

        // (keys[i] & 3) == 0
        const __m256i and3 = _mm256_and_si256(vk, vk_mask);
        const __m256i bit0 = _mm256_cmpeq_epi32(and3, _mm256_setzero_si256());

        // Combine: fmsk & kmask & bit0
        __m256i keep = _mm256_and_si256(fmsk, kmask);
        keep         = _mm256_and_si256(keep, bit0);

        // Zero-out unneeded lanes
        const __m256i vadd = _mm256_and_si256(vv, keep);
        vacc = _mm256_add_epi64(
            vacc,
            _mm256_unpacklo_epi32(vadd, _mm256_setzero_si256())
        );
        vacc = _mm256_add_epi64(
            vacc,
            _mm256_unpackhi_epi32(vadd, _mm256_setzero_si256())
        );
    }

    // Horizontal sum across 4 x 64-bit lanes
    __m256i hi = _mm256_srli_si256(vacc, 8);
    vacc = _mm256_add_epi64(vacc, hi);
    __m128i lo128 = _mm256_castsi256_si128(vacc);
    __m128i hi128 = _mm256_extracti128_si256(vacc, 1);
    lo128 = _mm_add_epi64(lo128, hi128);
    uint64_t acc = static_cast<uint64_t>(_mm_extract_epi64(lo128, 0)) +
                   static_cast<uint64_t>(_mm_extract_epi64(lo128, 1));

    // Scalar tail
    for (; i < n; ++i) {
        if (flags[i] && keys[i] < key_threshold && ((keys[i] & 3u) == 0u)) {
            acc += values[i];
        }
    }
    return acc;
}

uint64_t filter_sum_scalar(
    const uint32_t* __restrict keys,
    const uint32_t* __restrict values,
    const uint8_t* __restrict flags,
    uint32_t key_threshold,
    std::size_t n) noexcept
{
    uint64_t acc = 0;
    std::size_t i = 0;

    // 4-way unrolled scalar
    for (; i + 4 <= n; i += 4) {
        uint32_t k0 = keys[i];
        uint32_t k1 = keys[i+1];
        uint32_t k2 = keys[i+2];
        uint32_t k3 = keys[i+3];

        uint8_t f0 = flags[i];
        uint8_t f1 = flags[i+1];
        uint8_t f2 = flags[i+2];
        uint8_t f3 = flags[i+3];

        uint32_t m0 = (f0 && (k0 < key_threshold) && ((k0 & 3u) == 0u)) ? values[i]   : 0;
        uint32_t m1 = (f1 && (k1 < key_threshold) && ((k1 & 3u) == 0u)) ? values[i+1] : 0;
        uint32_t m2 = (f2 && (k2 < key_threshold) && ((k2 & 3u) == 0u)) ? values[i+2] : 0;
        uint32_t m3 = (f3 && (k3 < key_threshold) && ((k3 & 3u) == 0u)) ? values[i+3] : 0;

        acc += static_cast<uint64_t>(m0) + static_cast<uint64_t>(m1) +
               static_cast<uint64_t>(m2) + static_cast<uint64_t>(m3);
    }
    for (; i < n; ++i) {
        if (flags[i] && keys[i] < key_threshold && ((keys[i] & 3u) == 0u)) {
            acc += values[i];
        }
    }
    return acc;
}

} // anonymous

uint64_t filter_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint8_t>& flags,
    uint32_t key_threshold,
    int iters)
{
    const uint32_t* k = keys.data();
    const uint32_t* v = values.data();
    const uint8_t*  f = flags.data();
    const std::size_t n = keys.size();

    uint64_t total = 0;
    for (int iter = 0; iter < iters; ++iter) {
        if (cpu_has_avx2()) {
            total = filter_sum_avx2(k, v, f, key_threshold, n);
        } else {
            total = filter_sum_scalar(k, v, f, key_threshold, n);
        }
    }
    return total;
}