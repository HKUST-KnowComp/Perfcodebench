#include "interface.h"
#include <immintrin.h>
#include <cstdint>
#include <vector>

uint64_t filter_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint8_t>& flags,
    uint32_t key_threshold,
    int iters) {
    
    const size_t n = keys.size();
    const uint32_t* k_ptr = keys.data();
    const uint32_t* v_ptr = values.data();
    const uint8_t* f_ptr = flags.data();
    
    uint64_t final_sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        __m256i sum_v = _mm256_setzero_si256();
        size_t i = 0;

        // Process 8 elements at a time using AVX2
        for (; i + 7 < n; i += 8) {
            __m256i k = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(k_ptr + i));
            __m256i v = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(v_ptr + i));
            
            // Load 8 bytes of flags and expand to 8x32-bit integers
            __m128i f_small = _mm_loadl_epi64(reinterpret_cast<const __m128i*>(f_ptr + i));
            __m256i f = _mm256_cvtepu8_epi32(f_small);

            // Condition 1: flags[i] != 0
            __m256i mask_f = _mm256_cmpgt_epi32(f, _mm256_setzero_si256());

            // Condition 2: keys[i] < key_threshold
            // Note: _mm256_cmplt_epu32 is not in AVX2, so we use signed comparison with offset
            // or just use the fact that threshold is likely within signed range or use XOR trick.
            // For uint32 comparison: (a < b) is (a - 0x80000000 < b - 0x80000000) signed.
            __m256i offset = _mm256_set1_epi32(0x80000000);
            __m256i k_off = _mm256_xor_si256(k, offset);
            __m256i t_off = _mm256_xor_si256(_mm256_set1_epi32(key_threshold), offset);
            __m256i mask_k = _mm256_cmpgt_epi32(t_off, k_off);

            // Condition 3: (keys[i] & 3) == 0
            __m256i k_and = _mm256_and_si256(k, _mm256_set1_epi32(3));
            __m256i mask_mod = _mm256_cmpeq_epi32(k_and, _mm256_setzero_si256());

            // Combine masks
            __m256i mask = _mm256_and_si256(mask_f, _mm256_and_si256(mask_k, mask_mod));

            // Apply mask to values
            __m256i filtered_v = _mm256_and_si256(v, mask);

            // Widen 8x32-bit to 2x(4x64-bit) and add to sum_v
            __m256i v_lo = _mm256_cvtepu32_epi64(_mm256_extracti128_si256(filtered_v, 0));
            __m256i v_hi = _mm256_cvtepu32_epi64(_mm256_extracti128_si256(filtered_v, 1));
            
            sum_v = _mm256_add_epi64(sum_v, v_lo);
            sum_v = _mm256_add_epi64(sum_v, v_hi);
        }

        // Horizontal sum of the 4 uint64_t in sum_v
        alignas(32) uint64_t res[4];
        _mm256_store_si256(reinterpret_cast<__m256i*>(res), sum_v);
        uint64_t current_sum = res[0] + res[1] + res[2] + res[3];

        // Scalar tail
        for (; i < n; ++i) {
            if (f_ptr[i] && k_ptr[i] < key_threshold && ((k_ptr[i] & 3u) == 0u)) {
                current_sum += static_cast<uint64_t>(v_ptr[i]);
            }
        }
        final_sum = current_sum;
    }

    return final_sum;
}