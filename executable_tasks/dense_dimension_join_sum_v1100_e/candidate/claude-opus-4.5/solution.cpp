#include "interface.h"

#include <cstdint>
#include <vector>
#include <algorithm>
#include <immintrin.h>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {

    if (dim_keys.empty() || probe_keys.empty()) {
        return 0;
    }

    // Build direct lookup table: table[key - min_key] = value
    uint32_t min_key = dim_keys.front();
    uint32_t max_key = dim_keys.back();
    std::size_t table_size = static_cast<std::size_t>(max_key - min_key) + 1;
    
    std::vector<uint32_t> lookup(table_size, 0);
    for (std::size_t i = 0; i < dim_keys.size(); ++i) {
        lookup[dim_keys[i] - min_key] = dim_values[i];
    }

    const uint32_t* lookup_ptr = lookup.data();
    const uint32_t* probe_keys_ptr = probe_keys.data();
    const uint32_t* probe_weights_ptr = probe_weights.data();
    const std::size_t n = probe_keys.size();

    uint64_t sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;

#if defined(__AVX2__)
        __m256i vsum_lo = _mm256_setzero_si256();
        __m256i vsum_hi = _mm256_setzero_si256();
        __m256i vmin_key = _mm256_set1_epi32(static_cast<int>(min_key));

        std::size_t i = 0;
        std::size_t vec_end = (n / 8) * 8;

        for (; i < vec_end; i += 8) {
            // Load 8 probe keys and weights
            __m256i vkeys = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(probe_keys_ptr + i));
            __m256i vweights = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(probe_weights_ptr + i));

            // Compute indices: key - min_key
            __m256i vindices = _mm256_sub_epi32(vkeys, vmin_key);

            // Gather dim_values using indices
            __m256i vvalues = _mm256_i32gather_epi32(reinterpret_cast<const int*>(lookup_ptr), vindices, 4);

            // Multiply values * weights (32x32 -> 64 bit)
            // Low 4 elements
            __m256i vval_lo = _mm256_cvtepu32_epi64(_mm256_castsi256_si128(vvalues));
            __m256i vwgt_lo = _mm256_cvtepu32_epi64(_mm256_castsi256_si128(vweights));
            __m256i vprod_lo = _mm256_mul_epu32(vval_lo, vwgt_lo);
            vsum_lo = _mm256_add_epi64(vsum_lo, vprod_lo);

            // High 4 elements
            __m256i vval_hi = _mm256_cvtepu32_epi64(_mm256_extracti128_si256(vvalues, 1));
            __m256i vwgt_hi = _mm256_cvtepu32_epi64(_mm256_extracti128_si256(vweights, 1));
            __m256i vprod_hi = _mm256_mul_epu32(vval_hi, vwgt_hi);
            vsum_hi = _mm256_add_epi64(vsum_hi, vprod_hi);
        }

        // Horizontal sum of vsum_lo and vsum_hi
        __m256i vsum_total = _mm256_add_epi64(vsum_lo, vsum_hi);
        __m128i vsum_128 = _mm_add_epi64(_mm256_castsi256_si128(vsum_total),
                                          _mm256_extracti128_si256(vsum_total, 1));
        vsum_128 = _mm_add_epi64(vsum_128, _mm_srli_si128(vsum_128, 8));
        sum = static_cast<uint64_t>(_mm_cvtsi128_si64(vsum_128));

        // Scalar tail
        for (; i < n; ++i) {
            uint32_t idx = probe_keys_ptr[i] - min_key;
            sum += static_cast<uint64_t>(lookup_ptr[idx]) * static_cast<uint64_t>(probe_weights_ptr[i]);
        }
#else
        // Scalar fallback
        for (std::size_t i = 0; i < n; ++i) {
            uint32_t idx = probe_keys_ptr[i] - min_key;
            sum += static_cast<uint64_t>(lookup_ptr[idx]) * static_cast<uint64_t>(probe_weights_ptr[i]);
        }
#endif
    }

    return sum;
}
