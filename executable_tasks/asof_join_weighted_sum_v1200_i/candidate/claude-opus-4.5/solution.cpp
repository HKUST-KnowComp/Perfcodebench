#include "interface.h"

#include <cstdint>
#include <immintrin.h>

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
    
    const uint32_t* __restrict__ dim_ts_ptr = dim_ts.data();
    const uint32_t* __restrict__ dim_val_ptr = dim_values.data();
    const uint32_t* __restrict__ probe_ts_ptr = probe_ts.data();
    const uint32_t* __restrict__ probe_wt_ptr = probe_weights.data();
    
    // Pre-compute matched dimension indices using merge-join
    // This is O(probe_size + dim_size) instead of O(probe_size * log(dim_size))
    std::vector<uint32_t> matched_values(probe_size);
    uint32_t* __restrict__ matched_ptr = matched_values.data();
    
    uint64_t sum = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        
        // Merge-join: both arrays are sorted, so we can scan linearly
        std::size_t dim_idx = 0;
        const std::size_t dim_last = dim_size - 1;
        
        // Find initial position for first probe
        // Advance dim_idx to the largest index where dim_ts[dim_idx] <= probe_ts[0]
        std::size_t i = 0;
        
        // Process all probes with merge-join
        for (; i < probe_size; ++i) {
            const uint32_t pts = probe_ts_ptr[i];
            
            // Advance dim_idx while next dimension timestamp is still <= probe timestamp
            while (dim_idx < dim_last && dim_ts_ptr[dim_idx + 1] <= pts) {
                ++dim_idx;
            }
            
            matched_ptr[i] = dim_val_ptr[dim_idx];
        }
        
        // Now compute weighted sum with SIMD
        i = 0;
        const std::size_t simd_end = probe_size - (probe_size % 8);
        
        __m256i sum_lo = _mm256_setzero_si256();
        __m256i sum_hi = _mm256_setzero_si256();
        
        for (; i < simd_end; i += 8) {
            // Load 8 matched values and 8 weights
            __m256i vals = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(matched_ptr + i));
            __m256i wts = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(probe_wt_ptr + i));
            
            // Multiply: need 64-bit results from 32-bit inputs
            // _mm256_mul_epu32 multiplies lanes 0,2,4,6 and produces 64-bit results
            __m256i prod_lo = _mm256_mul_epu32(vals, wts);
            
            // Shift right by 32 bits to get lanes 1,3,5,7
            __m256i vals_hi = _mm256_srli_epi64(vals, 32);
            __m256i wts_hi = _mm256_srli_epi64(wts, 32);
            __m256i prod_hi = _mm256_mul_epu32(vals_hi, wts_hi);
            
            sum_lo = _mm256_add_epi64(sum_lo, prod_lo);
            sum_hi = _mm256_add_epi64(sum_hi, prod_hi);
        }
        
        // Horizontal sum of SIMD accumulators
        __m256i total = _mm256_add_epi64(sum_lo, sum_hi);
        
        // Extract and sum the 4 64-bit values
        uint64_t tmp[4];
        _mm256_storeu_si256(reinterpret_cast<__m256i*>(tmp), total);
        sum = tmp[0] + tmp[1] + tmp[2] + tmp[3];
        
        // Handle remaining elements
        for (; i < probe_size; ++i) {
            sum += static_cast<uint64_t>(matched_ptr[i]) * static_cast<uint64_t>(probe_wt_ptr[i]);
        }
    }
    
    return sum;
}