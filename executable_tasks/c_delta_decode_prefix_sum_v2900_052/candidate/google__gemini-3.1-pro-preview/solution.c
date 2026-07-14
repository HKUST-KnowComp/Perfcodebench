#include "interface.h"
#include <stdint.h>
#include <immintrin.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
    if (count == 0) return 0;

    uint64_t final_total = 0;
    const int64_t K = 0x9E3779B1LL;

    for (int iter = 0; iter < iters; ++iter) {
        int64_t weighted_sum = 0;
        size_t i = 0;

        // Use AVX2 to process 8 elements at a time
        // sum_v = sum_{i=0}^{count-1} (count - i) * deltas[i]
        __m256i sum_vec = _mm256_setzero_si256();

        for (; i + 7 < count; i += 8) {
            // Load 8 int32 deltas
            __m256i d_v = _mm256_loadu_si256((const __m256i*)&deltas[i]);

            // Weights: (count - i), (count - i - 1), ..., (count - i - 7)
            __m256i w_v_low = _mm256_set_epi64x(
                (int64_t)count - (i + 3), (int64_t)count - (i + 2),
                (int64_t)count - (i + 1), (int64_t)count - i
            );
            __m256i w_v_high = _mm256_set_epi64x(
                (int64_t)count - (i + 7), (int64_t)count - (i + 6),
                (int64_t)count - (i + 5), (int64_t)count - (i + 4)
            );

            // Sign-extend int32 to int64
            __m256i d_v_low = _mm256_cvtepi32_epi64(_mm256_castsi256_si128(d_v));
            __m256i d_v_high = _mm256_cvtepi32_epi64(_mm256_extracti128_si256(d_v, 1));

            // Multiply and accumulate
            __m256i prod_low = _mm256_mul_epi32(d_v_low, w_v_low);
            // Note: _mm256_mul_epi32 only multiplies the low 32 bits of each 64-bit lane.
            // Since our deltas are sign-extended int32s, the high 32 bits of d_v_low/high 
            // are just sign bits. However, _mm256_mul_epi32 is for 32x32->64.
            // We need full 64-bit multiplication or ensure 32-bit range.
            // Since deltas are int32 and weights are up to 2^31, product fits in int64.
            // Correct way for int64 * int64 in AVX2 is complex, but since deltas are 32-bit:
            // we can use _mm256_mul_epi32 if we ensure the 32-bit values are in the right place.
            
            // Re-evaluating: weights can be > 2^32 if count is large. 
            // Let's use a simpler scalar loop for the weighted sum or a more robust SIMD.
            // Given the task, a well-unrolled scalar loop is often very fast.
            
            weighted_sum += (int64_t)deltas[i] * (count - i);
            weighted_sum += (int64_t)deltas[i+1] * (count - (i + 1));
            weighted_sum += (int64_t)deltas[i+2] * (count - (i + 2));
            weighted_sum += (int64_t)deltas[i+3] * (count - (i + 3));
            weighted_sum += (int64_t)deltas[i+4] * (count - (i + 4));
            weighted_sum += (int64_t)deltas[i+5] * (count - (i + 5));
            weighted_sum += (int64_t)deltas[i+6] * (count - (i + 6));
            weighted_sum += (int64_t)deltas[i+7] * (count - (i + 7));
        }

        for (; i < count; ++i) {
            weighted_sum += (int64_t)deltas[i] * (count - i);
        }

        final_total = (uint64_t)weighted_sum + (uint64_t)count * (uint64_t)K;
    }

    return final_total;
}