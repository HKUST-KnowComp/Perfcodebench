#include "interface.h"

#include <cstddef>
#include <cstdint>
#include <immintrin.h>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {

    const std::size_t probe_n = probe_ts.size();
    const std::size_t dim_n = dim_ts.size();

    if (probe_n == 0 || dim_n == 0) {
        return 0;
    }

    const uint32_t* __restrict__ p_ts = probe_ts.data();
    const uint32_t* __restrict__ p_wt = probe_weights.data();
    const uint32_t* __restrict__ d_ts = dim_ts.data();
    const uint32_t* __restrict__ d_val = dim_values.data();

    // Pre-compute matched dim_values for each probe using merge join (O(n+m))
    // We'll store the matched value indices or values directly.
    // Since we iterate multiple times, caching matched values is beneficial.
    std::vector<uint32_t> matched_vals(probe_n);

    {
        std::size_t d_idx = 0;
        for (std::size_t i = 0; i < probe_n; ++i) {
            uint32_t ts = p_ts[i];
            // Advance d_idx while next dim timestamp <= ts
            while (d_idx + 1 < dim_n && d_ts[d_idx + 1] <= ts) {
                ++d_idx;
            }
            matched_vals[i] = d_val[d_idx];
        }
    }

    const uint32_t* __restrict__ m_val = matched_vals.data();

    uint64_t sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;

#if defined(__AVX2__)
        __m256i acc_lo = _mm256_setzero_si256();
        __m256i acc_hi = _mm256_setzero_si256();

        std::size_t i = 0;
        const std::size_t vec_end = probe_n & ~std::size_t(7);

        for (; i < vec_end; i += 8) {
            __m256i vals = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(m_val + i));
            __m256i wts  = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(p_wt + i));

            // 32x32->64 multiply: need to split into even/odd lanes
            // _mm256_mul_epu32 multiplies lanes 0,2,4,6 (treating as 64-bit pairs)
            __m256i prod_even = _mm256_mul_epu32(vals, wts);

            __m256i vals_odd = _mm256_srli_epi64(vals, 32);
            __m256i wts_odd  = _mm256_srli_epi64(wts, 32);
            __m256i prod_odd = _mm256_mul_epu32(vals_odd, wts_odd);

            acc_lo = _mm256_add_epi64(acc_lo, prod_even);
            acc_hi = _mm256_add_epi64(acc_hi, prod_odd);
        }

        // Horizontal sum of accumulators
        __m256i acc_total = _mm256_add_epi64(acc_lo, acc_hi);
        // acc_total has 4 x 64-bit values
        __m128i lo128 = _mm256_castsi256_si128(acc_total);
        __m128i hi128 = _mm256_extracti128_si256(acc_total, 1);
        __m128i sum128 = _mm_add_epi64(lo128, hi128);
        // sum128 has 2 x 64-bit values
        uint64_t s0 = static_cast<uint64_t>(_mm_extract_epi64(sum128, 0));
        uint64_t s1 = static_cast<uint64_t>(_mm_extract_epi64(sum128, 1));
        sum = s0 + s1;

        // Scalar tail
        for (; i < probe_n; ++i) {
            sum += static_cast<uint64_t>(m_val[i]) * static_cast<uint64_t>(p_wt[i]);
        }
#else
        for (std::size_t i = 0; i < probe_n; ++i) {
            sum += static_cast<uint64_t>(m_val[i]) * static_cast<uint64_t>(p_wt[i]);
        }
#endif
    }

    return sum;
}
