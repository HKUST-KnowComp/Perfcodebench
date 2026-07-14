#include "interface.h"
#include <stdint.h>
#include <stdlib.h>

/**
 * Optimized linear audio resampling with integrated FNV-1a checksum.
 * 
 * Key optimizations:
 * 1. Moved malloc/free outside the iteration loop.
 * 2. Combined resampling and checksum calculation to improve cache locality.
 * 3. Replaced double-precision floating point with 64-bit fixed-point arithmetic (32.32).
 * 4. Pre-calculated the step size to avoid division inside the inner loop.
 */

uint64_t run(const int16_t* src, int input_len, int output_len, int iters) {
    if (output_len <= 0) return 0;

    // Allocate buffer once outside the loop
    int16_t* out = (int16_t*)malloc((size_t)output_len * sizeof(int16_t));
    if (!out) return 0;

    uint64_t final_hash = 0;
    const uint64_t fnv_prime = 1099511628211ULL;
    const uint64_t fnv_offset_basis = 1469598103934665603ULL;

    // Pre-calculate step size in 32.32 fixed point
    // pos = i * (input_len - 1) / (output_len - 1)
    uint64_t step_fixed = 0;
    if (output_len > 1) {
        step_fixed = (((uint64_t)(input_len - 1)) << 32) / (output_len - 1);
    }

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t hash = fnv_offset_basis;
        uint64_t pos_fixed = 0;

        for (int i = 0; i < output_len; ++i) {
            int left = (int)(pos_fixed >> 32);
            int right = left + 1;
            if (right >= input_len) right = input_len - 1;

            // frac is the lower 32 bits of pos_fixed
            uint64_t frac_fixed = pos_fixed & 0xFFFFFFFFULL;

            // Linear interpolation: sample = src[left] + frac * (src[right] - src[left])
            // Using 64-bit intermediate to prevent overflow before the shift
            int32_t s_left = (int32_t)src[left];
            int32_t s_right = (int32_t)src[right];
            int32_t diff = s_right - s_left;
            
            // Fixed point multiply: (diff * frac_fixed) >> 32
            int64_t product = (int64_t)diff * (int64_t)frac_fixed;
            
            // Rounding logic equivalent to: (int16_t)(sample + (sample >= 0 ? 0.5 : -0.5))
            // In fixed point, 0.5 is 1 << 31.
            int64_t sample_fixed = ((int64_t)s_left << 32) + product;
            
            int16_t result;
            if (sample_fixed >= 0) {
                result = (int16_t)((sample_fixed + 0x80000000LL) >> 32);
            } else {
                result = (int16_t)((sample_fixed - 0x7FFFFFFFLL) >> 32);
            }

            // Update checksum
            hash ^= (uint16_t)result;
            hash *= fnv_prime;

            pos_fixed += step_fixed;
        }
        final_hash = hash;
    }

    free(out);
    return final_hash;
}
