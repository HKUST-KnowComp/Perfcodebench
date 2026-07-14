#include "interface.h"

#include <cstdint>
#include <cstddef>

#if defined(__AVX2__)
#include <immintrin.h>
#endif

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

    uint64_t sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        std::size_t d_idx = 0;
        std::size_t p_idx = 0;

#if defined(__AVX2__)
        // Precompute matched dim indices for all probes using merge join
        // Then vectorize the multiply-accumulate
        alignas(32) uint32_t matched_vals[8];
        alignas(32) uint32_t weights_buf[8];

        __m256i sum_lo = _mm256_setzero_si256();
        __m256i sum_hi = _mm256_setzero_si256();

        while (p_idx + 8 <= probe_n) {
            // Advance dim pointer and gather 8 matched values
            for (int k = 0; k < 8; ++k) {
                uint32_t pts = p_ts[p_idx + k];
                while (d_idx + 1 < dim_n && d_ts[d_idx + 1] <= pts) {
                    ++d_idx;
                }
                matched_vals[k] = d_val[d_idx];
                weights_buf[k] = p_wt[p_idx + k];
            }

            __m256i vals = _mm256_load_si256(reinterpret_cast<const __m256i*>(matched_vals));
            __m256i wts = _mm256_load_si256(reinterpret_cast<const __m256i*>(weights_buf));

            // Multiply: 32x32 -> 64 bit
            // _mm256_mul_epu32 multiplies lanes 0,2,4,6 (treating as 64-bit pairs)
            // We need to handle all 8 lanes
            __m256i vals_lo = _mm256_and_si256(vals, _mm256_set1_epi64x(0xFFFFFFFF));
            __m256i vals_hi = _mm256_srli_epi64(vals, 32);
            __m256i wts_lo = _mm256_and_si256(wts, _mm256_set1_epi64x(0xFFFFFFFF));
            __m256i wts_hi = _mm256_srli_epi64(wts, 32);

            __m256i prod_lo = _mm256_mul_epu32(vals_lo, wts_lo);
            __m256i prod_hi = _mm256_mul_epu32(vals_hi, wts_hi);

            sum_lo = _mm256_add_epi64(sum_lo, prod_lo);
            sum_hi = _mm256_add_epi64(sum_hi, prod_hi);

            p_idx += 8;
        }

        // Horizontal sum
        __m256i total = _mm256_add_epi64(sum_lo, sum_hi);
        alignas(32) uint64_t tmp[4];
        _mm256_store_si256(reinterpret_cast<__m256i*>(tmp), total);
        sum = tmp[0] + tmp[1] + tmp[2] + tmp[3];

        // Scalar tail
        for (; p_idx < probe_n; ++p_idx) {
            uint32_t pts = p_ts[p_idx];
            while (d_idx + 1 < dim_n && d_ts[d_idx + 1] <= pts) {
                ++d_idx;
            }
            sum += static_cast<uint64_t>(d_val[d_idx]) * static_cast<uint64_t>(p_wt[p_idx]);
        }
#else
        // Scalar merge join
        for (; p_idx < probe_n; ++p_idx) {
            uint32_t pts = p_ts[p_idx];
            while (d_idx + 1 < dim_n && d_ts[d_idx + 1] <= pts) {
                ++d_idx;
            }
            sum += static_cast<uint64_t>(d_val[d_idx]) * static_cast<uint64_t>(p_wt[p_idx]);
        }
#endif
    }

    return sum;
}
