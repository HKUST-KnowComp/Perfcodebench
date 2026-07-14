#include "interface.h"
#include <stdint.h>
#include <immintrin.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
    if (count == 0) return 0;

    const uint64_t constant_offset = 0x9E3779B1LL;
    uint64_t final_total = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t total = 0;
        size_t i = 0;

        // Use SIMD to calculate sum((count - i) * deltas[i])
        // We process 8 elements at a time using AVX2.
        __m256i total_vec = _mm256_setzero_si256();
        
        for (; i + 7 < count; i += 8) {
            // Load 8 int32_t deltas
            __m256i d_32 = _mm256_loadu_si256((const __m256i*)&deltas[i]);

            // Prepare weights (count - i, count - i - 1, ...)
            __m256i weights_low = _mm256_set_epi64x(
                (int64_t)(count - (i + 3)),
                (int64_t)(count - (i + 2)),
                (int64_t)(count - (i + 1)),
                (int64_t)(count - i)
            );
            __m256i weights_high = _mm256_set_epi64x(
                (int64_t)(count - (i + 7)),
                (int64_t)(count - (i + 6)),
                (int64_t)(count - (i + 5)),
                (int64_t)(count - (i + 4))
            );

            // Sign-extend 32-bit deltas to 64-bit
            __m256i d_64_low = _mm256_cvtepi32_epi64(_mm256_castsi256_si128(d_32));
            __m256i d_64_high = _mm256_cvtepi32_epi64(_mm256_extracti128_si256(d_32, 1));

            // Multiply weights by deltas and accumulate
            __m256i prod_low = _mm256_mul_epi32(d_64_low, weights_low);
            __m256i prod_high = _mm256_mul_epi32(d_64_high, weights_high);
            
            // Note: _mm256_mul_epi32 only multiplies the lower 32 bits of each 64-bit lane.
            // Since our deltas are sign-extended to 64-bit, the values are in the lower 32 bits.
            // However, for negative numbers, we need full 64-bit multiplication or careful handling.
            // A safer way for signed 64-bit multiplication in AVX2 is not direct, 
            // but since deltas are 32-bit, we can use _mm256_mul_epi32 if we ensure the sign extension is handled.
            // Actually, for signed 32-bit * 64-bit, we can just use the fact that deltas fit in 32 bits.
            
            // Correcting to full 64-bit multiplication logic:
            // Since weights can be large, we use the property that deltas are 32-bit.
            // _mm256_mul_epi32 is perfect here because it takes the bottom 32 bits of each 64-bit lane.
            total_vec = _mm256_add_epi64(total_vec, prod_low);
            total_vec = _mm256_add_epi64(total_vec, prod_high);
        }

        // Horizontal sum of the SIMD register
        uint64_t temp_sums[4];
        _mm256_storeu_si256((__m256i*)temp_sums, total_vec);
        total = temp_sums[0] + temp_sums[1] + temp_sums[2] + temp_sums[3];

        // Handle remaining elements
        for (; i < count; ++i) {
            total += (uint64_t)((int64_t)deltas[i] * (int64_t)(count - i));
        }

        // Add the constant offset part: sum_{i=0}^{count-1} constant_offset
        total += (uint64_t)count * constant_offset;
        final_total = total;
    }

    return final_total;
}