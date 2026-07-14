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

    const std::size_t probe_size = probe_ts.size();
    const std::size_t dim_size = dim_ts.size();

    if (probe_size == 0 || dim_size == 0) {
        return 0;
    }

    const uint32_t* __restrict__ p_ts = probe_ts.data();
    const uint32_t* __restrict__ p_wt = probe_weights.data();
    const uint32_t* __restrict__ d_ts = dim_ts.data();
    const uint32_t* __restrict__ d_val = dim_values.data();

    // Precompute matched dimension values for each probe using linear merge
    std::vector<uint32_t> matched_values(probe_size);
    uint32_t* __restrict__ mv = matched_values.data();

    {
        std::size_t d_idx = 0;
        uint32_t cur_val = d_val[0];
        for (std::size_t i = 0; i < probe_size; ++i) {
            uint32_t ts = p_ts[i];
            // Advance d_idx while next dimension timestamp <= probe timestamp
            while (d_idx + 1 < dim_size && d_ts[d_idx + 1] <= ts) {
                ++d_idx;
            }
            cur_val = d_val[d_idx];
            mv[i] = cur_val;
        }
    }

    uint64_t sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t local_sum = 0;

#if defined(__AVX2__)
        // Process 4 elements at a time using AVX2
        __m256i acc = _mm256_setzero_si256();
        std::size_t i = 0;
        const std::size_t vec_end = probe_size & ~std::size_t(3);

        for (; i < vec_end; i += 4) {
            // Load 4 x uint32 values and weights
            __m128i vals = _mm_loadu_si128(reinterpret_cast<const __m128i*>(mv + i));
            __m128i wts = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p_wt + i));

            // Zero-extend to 64-bit
            __m256i vals64 = _mm256_cvtepu32_epi64(vals);
            __m256i wts64 = _mm256_cvtepu32_epi64(wts);

            // Multiply and accumulate
            __m256i prod = _mm256_mul_epu32(vals64, wts64);
            acc = _mm256_add_epi64(acc, prod);
        }

        // Horizontal sum of acc
        alignas(32) uint64_t tmp[4];
        _mm256_store_si256(reinterpret_cast<__m256i*>(tmp), acc);
        local_sum = tmp[0] + tmp[1] + tmp[2] + tmp[3];

        // Scalar tail
        for (; i < probe_size; ++i) {
            local_sum += static_cast<uint64_t>(mv[i]) * static_cast<uint64_t>(p_wt[i]);
        }
#else
        // Scalar fallback
        for (std::size_t i = 0; i < probe_size; ++i) {
            local_sum += static_cast<uint64_t>(mv[i]) * static_cast<uint64_t>(p_wt[i]);
        }
#endif

        sum = local_sum;
    }

    return sum;
}
