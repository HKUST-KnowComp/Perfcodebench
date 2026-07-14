#include "interface.h"

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

    uint64_t sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        std::size_t dim_idx = 0;

        // Phase 1: linear merge to build matched dim_values for each probe
        // We'll accumulate directly to avoid extra memory traffic.

        __m256i acc0 = _mm256_setzero_si256();
        __m256i acc1 = _mm256_setzero_si256();

        std::size_t i = 0;
        const std::size_t vec_end = (probe_n >= 8) ? (probe_n - 7) : 0;

        // We need to track dim_idx per probe. For vectorization we process
        // 8 probes at a time but dim_idx advances monotonically.
        // Strategy: advance dim_idx for each probe in scalar, but accumulate
        // products in SIMD batches.

        // Actually, since both are sorted, we can do a two-pointer merge
        // and batch the multiply-accumulate.

        // Collect matched values in a small buffer, then SIMD reduce.
        alignas(32) uint32_t val_buf[8];
        alignas(32) uint32_t wt_buf[8];

        while (i < vec_end) {
            // Fill buffer with 8 matched values
            for (int k = 0; k < 8; ++k) {
                uint32_t pts = p_ts[i + k];
                // Advance dim_idx while next dim_ts <= pts
                while (dim_idx + 1 < dim_n && d_ts[dim_idx + 1] <= pts) {
                    ++dim_idx;
                }
                val_buf[k] = d_val[dim_idx];
                wt_buf[k] = p_wt[i + k];
            }

            // Load and multiply
            __m256i v_val = _mm256_load_si256(reinterpret_cast<const __m256i*>(val_buf));
            __m256i v_wt  = _mm256_load_si256(reinterpret_cast<const __m256i*>(wt_buf));

            // 32x32->64 multiply: need to split into even/odd lanes
            __m256i v_val_lo = _mm256_and_si256(v_val, _mm256_set1_epi64x(0xFFFFFFFF));
            __m256i v_wt_lo  = _mm256_and_si256(v_wt,  _mm256_set1_epi64x(0xFFFFFFFF));
            __m256i prod_lo  = _mm256_mul_epu32(v_val_lo, v_wt_lo);

            __m256i v_val_hi = _mm256_srli_epi64(v_val, 32);
            __m256i v_wt_hi  = _mm256_srli_epi64(v_wt,  32);
            __m256i prod_hi  = _mm256_mul_epu32(v_val_hi, v_wt_hi);

            acc0 = _mm256_add_epi64(acc0, prod_lo);
            acc1 = _mm256_add_epi64(acc1, prod_hi);

            i += 8;
        }

        // Horizontal sum of accumulators
        __m256i acc = _mm256_add_epi64(acc0, acc1);
        __m128i lo128 = _mm256_castsi256_si128(acc);
        __m128i hi128 = _mm256_extracti128_si256(acc, 1);
        __m128i sum128 = _mm_add_epi64(lo128, hi128);
        sum = static_cast<uint64_t>(_mm_extract_epi64(sum128, 0)) +
              static_cast<uint64_t>(_mm_extract_epi64(sum128, 1));

        // Scalar tail
        for (; i < probe_n; ++i) {
            uint32_t pts = p_ts[i];
            while (dim_idx + 1 < dim_n && d_ts[dim_idx + 1] <= pts) {
                ++dim_idx;
            }
            sum += static_cast<uint64_t>(d_val[dim_idx]) * static_cast<uint64_t>(p_wt[i]);
        }
    }

    return sum;
}
