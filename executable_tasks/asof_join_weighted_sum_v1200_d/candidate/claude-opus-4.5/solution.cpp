#include "interface.h"

#include <cstdint>
#include <immintrin.h>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {

    const std::size_t dim_size = dim_ts.size();
    const std::size_t probe_size = probe_ts.size();

    if (probe_size == 0 || dim_size == 0) {
        return 0;
    }

    const uint32_t* __restrict__ dim_ts_ptr = dim_ts.data();
    const uint32_t* __restrict__ dim_val_ptr = dim_values.data();
    const uint32_t* __restrict__ probe_ts_ptr = probe_ts.data();
    const uint32_t* __restrict__ probe_wt_ptr = probe_weights.data();

    uint64_t sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        std::size_t dim_idx = 0;
        std::size_t i = 0;

        // Skip probes before first dim timestamp
        while (i < probe_size && probe_ts_ptr[i] < dim_ts_ptr[0]) {
            ++i;
        }

        // Advance dim_idx to the correct starting position
        while (dim_idx + 1 < dim_size && dim_ts_ptr[dim_idx + 1] <= probe_ts_ptr[i]) {
            ++dim_idx;
        }

        // SIMD accumulator
        __m256i acc_lo = _mm256_setzero_si256();
        __m256i acc_hi = _mm256_setzero_si256();

        // Process in chunks where dim_idx stays constant
        while (i < probe_size) {
            // Find the range of probes that map to current dim_idx
            uint32_t upper_bound = (dim_idx + 1 < dim_size) ? dim_ts_ptr[dim_idx + 1] : UINT32_MAX;
            uint32_t cur_val = dim_val_ptr[dim_idx];
            __m256i val_vec = _mm256_set1_epi32(static_cast<int32_t>(cur_val));

            // Process probes in this range with SIMD
            while (i + 8 <= probe_size) {
                // Prefetch ahead
                _mm_prefetch(reinterpret_cast<const char*>(probe_ts_ptr + i + 64), _MM_HINT_T0);
                _mm_prefetch(reinterpret_cast<const char*>(probe_wt_ptr + i + 64), _MM_HINT_T0);

                // Check if all 8 probes are within current dim range
                if (probe_ts_ptr[i + 7] < upper_bound) {
                    // All 8 probes map to current dim_idx
                    __m256i weights = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(probe_wt_ptr + i));
                    
                    // Multiply and accumulate as 64-bit
                    __m256i prod_lo = _mm256_mul_epu32(val_vec, weights);
                    __m256i val_hi = _mm256_srli_epi64(val_vec, 32);
                    __m256i wt_hi = _mm256_srli_epi64(weights, 32);
                    __m256i prod_hi = _mm256_mul_epu32(val_hi, wt_hi);
                    
                    acc_lo = _mm256_add_epi64(acc_lo, prod_lo);
                    acc_hi = _mm256_add_epi64(acc_hi, prod_hi);
                    
                    i += 8;
                } else {
                    // Mixed range, process scalar
                    break;
                }
            }

            // Scalar processing for remainder or mixed ranges
            while (i < probe_size && probe_ts_ptr[i] < upper_bound) {
                sum += static_cast<uint64_t>(cur_val) * static_cast<uint64_t>(probe_wt_ptr[i]);
                ++i;
            }

            // Advance dim_idx for next probe
            if (i < probe_size) {
                uint32_t next_ts = probe_ts_ptr[i];
                while (dim_idx + 1 < dim_size && dim_ts_ptr[dim_idx + 1] <= next_ts) {
                    ++dim_idx;
                }
            }
        }

        // Reduce SIMD accumulators
        __m256i acc_total = _mm256_add_epi64(acc_lo, acc_hi);
        alignas(32) uint64_t acc_arr[4];
        _mm256_store_si256(reinterpret_cast<__m256i*>(acc_arr), acc_total);
        sum += acc_arr[0] + acc_arr[1] + acc_arr[2] + acc_arr[3];
    }

    return sum;
}
