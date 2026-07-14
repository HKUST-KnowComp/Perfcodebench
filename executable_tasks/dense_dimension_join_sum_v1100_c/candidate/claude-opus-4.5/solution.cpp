#include "interface.h"

#include <cstdint>
#include <cstring>
#include <vector>
#include <algorithm>

#if defined(__x86_64__) || defined(_M_X64)
#include <immintrin.h>
#define HAS_AVX2 1
#else
#define HAS_AVX2 0
#endif

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {

    if (dim_keys.empty() || probe_keys.empty()) {
        return 0;
    }

    // Build direct-address lookup table
    uint32_t min_key = dim_keys[0];
    uint32_t max_key = dim_keys[0];
    for (size_t i = 1; i < dim_keys.size(); ++i) {
        if (dim_keys[i] < min_key) min_key = dim_keys[i];
        if (dim_keys[i] > max_key) max_key = dim_keys[i];
    }

    const size_t table_size = static_cast<size_t>(max_key - min_key) + 1;
    std::vector<uint32_t> lookup(table_size, 0);

    for (size_t i = 0; i < dim_keys.size(); ++i) {
        lookup[dim_keys[i] - min_key] = dim_values[i];
    }

    const uint32_t* lookup_ptr = lookup.data();
    const uint32_t* probe_keys_ptr = probe_keys.data();
    const uint32_t* probe_weights_ptr = probe_weights.data();
    const size_t n = probe_keys.size();

    uint64_t sum = 0;

#if HAS_AVX2
    const __m256i vmin_key = _mm256_set1_epi32(static_cast<int32_t>(min_key));

    for (int iter = 0; iter < iters; ++iter) {
        __m256i vsum0 = _mm256_setzero_si256();
        __m256i vsum1 = _mm256_setzero_si256();
        __m256i vsum2 = _mm256_setzero_si256();
        __m256i vsum3 = _mm256_setzero_si256();

        size_t i = 0;
        const size_t vec_end = (n / 32) * 32;

        for (; i < vec_end; i += 32) {
            // Prefetch ahead
            if (i + 128 < n) {
                _mm_prefetch(reinterpret_cast<const char*>(probe_keys_ptr + i + 128), _MM_HINT_T0);
                _mm_prefetch(reinterpret_cast<const char*>(probe_weights_ptr + i + 128), _MM_HINT_T0);
            }

            // Load 32 keys and weights
            __m256i keys0 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(probe_keys_ptr + i));
            __m256i keys1 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(probe_keys_ptr + i + 8));
            __m256i keys2 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(probe_keys_ptr + i + 16));
            __m256i keys3 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(probe_keys_ptr + i + 24));

            __m256i weights0 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(probe_weights_ptr + i));
            __m256i weights1 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(probe_weights_ptr + i + 8));
            __m256i weights2 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(probe_weights_ptr + i + 16));
            __m256i weights3 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(probe_weights_ptr + i + 24));

            // Compute indices
            __m256i idx0 = _mm256_sub_epi32(keys0, vmin_key);
            __m256i idx1 = _mm256_sub_epi32(keys1, vmin_key);
            __m256i idx2 = _mm256_sub_epi32(keys2, vmin_key);
            __m256i idx3 = _mm256_sub_epi32(keys3, vmin_key);

            // Gather dim_values
            __m256i vals0 = _mm256_i32gather_epi32(reinterpret_cast<const int*>(lookup_ptr), idx0, 4);
            __m256i vals1 = _mm256_i32gather_epi32(reinterpret_cast<const int*>(lookup_ptr), idx1, 4);
            __m256i vals2 = _mm256_i32gather_epi32(reinterpret_cast<const int*>(lookup_ptr), idx2, 4);
            __m256i vals3 = _mm256_i32gather_epi32(reinterpret_cast<const int*>(lookup_ptr), idx3, 4);

            // Multiply and accumulate (32-bit x 32-bit -> 64-bit)
            // Process low and high halves separately
            __m256i prod0_lo = _mm256_mul_epu32(vals0, weights0);
            __m256i prod0_hi = _mm256_mul_epu32(_mm256_srli_epi64(vals0, 32), _mm256_srli_epi64(weights0, 32));
            __m256i prod1_lo = _mm256_mul_epu32(vals1, weights1);
            __m256i prod1_hi = _mm256_mul_epu32(_mm256_srli_epi64(vals1, 32), _mm256_srli_epi64(weights1, 32));
            __m256i prod2_lo = _mm256_mul_epu32(vals2, weights2);
            __m256i prod2_hi = _mm256_mul_epu32(_mm256_srli_epi64(vals2, 32), _mm256_srli_epi64(weights2, 32));
            __m256i prod3_lo = _mm256_mul_epu32(vals3, weights3);
            __m256i prod3_hi = _mm256_mul_epu32(_mm256_srli_epi64(vals3, 32), _mm256_srli_epi64(weights3, 32));

            vsum0 = _mm256_add_epi64(vsum0, prod0_lo);
            vsum0 = _mm256_add_epi64(vsum0, prod0_hi);
            vsum1 = _mm256_add_epi64(vsum1, prod1_lo);
            vsum1 = _mm256_add_epi64(vsum1, prod1_hi);
            vsum2 = _mm256_add_epi64(vsum2, prod2_lo);
            vsum2 = _mm256_add_epi64(vsum2, prod2_hi);
            vsum3 = _mm256_add_epi64(vsum3, prod3_lo);
            vsum3 = _mm256_add_epi64(vsum3, prod3_hi);
        }

        // Combine vector sums
        __m256i vsum_all = _mm256_add_epi64(_mm256_add_epi64(vsum0, vsum1), _mm256_add_epi64(vsum2, vsum3));
        
        // Horizontal sum
        __m128i lo = _mm256_castsi256_si128(vsum_all);
        __m128i hi = _mm256_extracti128_si256(vsum_all, 1);
        __m128i sum128 = _mm_add_epi64(lo, hi);
        sum = static_cast<uint64_t>(_mm_extract_epi64(sum128, 0)) + static_cast<uint64_t>(_mm_extract_epi64(sum128, 1));

        // Scalar tail
        for (; i < n; ++i) {
            uint32_t idx = probe_keys_ptr[i] - min_key;
            sum += static_cast<uint64_t>(lookup_ptr[idx]) * static_cast<uint64_t>(probe_weights_ptr[i]);
        }
    }
#else
    // Scalar fallback with unrolling
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        size_t i = 0;
        const size_t n4 = (n / 4) * 4;
        
        for (; i < n4; i += 4) {
            uint32_t idx0 = probe_keys_ptr[i] - min_key;
            uint32_t idx1 = probe_keys_ptr[i + 1] - min_key;
            uint32_t idx2 = probe_keys_ptr[i + 2] - min_key;
            uint32_t idx3 = probe_keys_ptr[i + 3] - min_key;
            
            sum += static_cast<uint64_t>(lookup_ptr[idx0]) * static_cast<uint64_t>(probe_weights_ptr[i]);
            sum += static_cast<uint64_t>(lookup_ptr[idx1]) * static_cast<uint64_t>(probe_weights_ptr[i + 1]);
            sum += static_cast<uint64_t>(lookup_ptr[idx2]) * static_cast<uint64_t>(probe_weights_ptr[i + 2]);
            sum += static_cast<uint64_t>(lookup_ptr[idx3]) * static_cast<uint64_t>(probe_weights_ptr[i + 3]);
        }
        
        for (; i < n; ++i) {
            uint32_t idx = probe_keys_ptr[i] - min_key;
            sum += static_cast<uint64_t>(lookup_ptr[idx]) * static_cast<uint64_t>(probe_weights_ptr[i]);
        }
    }
#endif

    return sum;
}