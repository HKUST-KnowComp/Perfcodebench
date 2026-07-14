#include "interface.h"
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

/**
 * Optimized linear audio resampling and checksum calculation.
 * 
 * Key optimizations:
 * 1. Removed malloc/free inside the iteration loop.
 * 2. Merged the resampling and checksum loops to reduce memory traffic and improve cache locality.
 * 3. Pre-calculated the resampling step to avoid repeated divisions.
 * 4. Optimized the rounding logic for int16_t conversion.
 */
uint64_t run(const int16_t* src, int input_len, int output_len, int iters) {
    if (output_len <= 0) return 0;

    uint64_t hash = 0;
    const double step = (output_len > 1) 
        ? (double)(input_len - 1) / (double)(output_len - 1) 
        : 0.0;
    const uint64_t fnv_prime = 1099511628211ULL;
    const uint64_t fnv_offset = 1469598103934665603ULL;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_hash = fnv_offset;

        for (int i = 0; i < output_len; ++i) {
            double pos = (double)i * step;
            int left = (int)pos;
            int right = left + 1;
            if (right >= input_len) right = input_len - 1;
            
            double frac = pos - (double)left;
            double s_left = (double)src[left];
            double s_right = (double)src[right];
            double sample = s_left + (s_right - s_left) * frac;

            int16_t rounded_sample;
            if (sample >= 0.0) {
                rounded_sample = (int16_t)(sample + 0.5);
            } else {
                rounded_sample = (int16_t)(sample - 0.5);
            }

            // FNV-1a checksum update
            current_hash ^= (uint16_t)rounded_sample;
            current_hash *= fnv_prime;
        }
        hash = current_hash;
    }

    return hash;
}
