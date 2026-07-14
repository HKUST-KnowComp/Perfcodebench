#include "interface.h"
#include <immintrin.h>
#include <cstdint>
#include <vector>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {

    if (probe_keys.empty() || dim_keys.empty()) return 0;

    const uint32_t min_key = dim_keys[0];
    const size_t probe_size = probe_keys.size();
    const uint32_t* p_keys = probe_keys.data();
    const uint32_t* p_weights = probe_weights.data();
    const uint32_t* d_values = dim_values.data();

    // Check if keys are perfectly dense (contiguous range)
    bool is_dense = (dim_keys.size() > 0 && (dim_keys.back() - dim_keys.front() + 1 == dim_keys.size()));

    uint64_t final_sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;

        if (is_dense) {
            size_t i = 0;
            __m256i sum_v = _mm256_setzero_si256();
            __m256i min_key_v = _mm256_set1_epi32(min_key);

            // Process 8 elements at a time using AVX2
            for (; i + 7 < probe_size; i += 8) {
                __m256i pk = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(p_keys + i));
                __m256i pw = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(p_weights + i));
                
                // Calculate indices: idx = probe_key - min_key
                __m256i idx = _mm256_sub_epi32(pk, min_key_v);

                // Gather dim_values[idx]
                __m256i dv = _mm256_i32gather_epi32(reinterpret_cast<const int*>(d_values), idx, 4);

                // Multiply dv * pw (32-bit * 32-bit -> 64-bit)
                // Split into two 4-element 64-bit vectors
                __m256i prod_lo = _mm256_mul_epu32(dv, pw);
                __m256i prod_hi = _mm256_mul_epu32(_mm256_srli_si256(dv, 4), _mm256_srli_si256(pw, 4));

                sum_v = _mm256_add_epi64(sum_v, prod_lo);
                sum_v = _mm256_add_epi64(sum_v, prod_hi);
            }

            // Horizontal sum of AVX register
            uint64_t temp[4];
            _mm256_storeu_si256(reinterpret_cast<__m256i*>(temp), sum_v);
            current_sum = temp[0] + temp[1] + temp[2] + temp[3];

            // Scalar tail
            for (; i < probe_size; ++i) {
                current_sum += static_cast<uint64_t>(d_values[p_keys[i] - min_key]) * p_weights[i];
            }
        } else {
            // Fallback for non-dense keys (binary search as in baseline)
            for (size_t i = 0; i < probe_size; ++i) {
                auto it = std::lower_bound(dim_keys.begin(), dim_keys.end(), p_keys[i]);
                size_t idx = static_cast<size_t>(it - dim_keys.begin());
                current_sum += static_cast<uint64_t>(dim_values[idx]) * p_weights[i];
            }
        }
        final_sum = current_sum;
    }

    return final_sum;
}