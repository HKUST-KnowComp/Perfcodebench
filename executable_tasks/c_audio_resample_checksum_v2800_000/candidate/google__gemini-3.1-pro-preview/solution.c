#include "interface.h"
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

static inline uint64_t checksum_i16(const int16_t* data, size_t n) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < n; ++i) {
        hash ^= (uint16_t)data[i];
        hash *= 1099511628211ULL;
    }
    return hash;
}

uint64_t run(const int16_t* src, int input_len, int output_len, int iters) {
    if (output_len <= 0) return 0;
    
    int16_t* out = (int16_t*)malloc((size_t)output_len * sizeof(int16_t));
    if (!out) return 0;

    uint64_t hash = 0;
    
    // Handle edge case where output_len is 1 to avoid division by zero
    if (output_len == 1) {
        for (int iter = 0; iter < iters; ++iter) {
            out[0] = src[0];
            hash = checksum_i16(out, 1);
        }
        free(out);
        return hash;
    }

    const double factor = (double)(input_len - 1) / (double)(output_len - 1);
    const int last_idx = input_len - 1;

    for (int iter = 0; iter < iters; ++iter) {
        // Resampling loop
        for (int i = 0; i < output_len; ++i) {
            double pos = (double)i * factor;
            int left = (int)pos;
            int right = left + 1;
            if (right > last_idx) right = last_idx;
            
            double frac = pos - (double)left;
            double s_left = (double)src[left];
            double s_right = (double)src[right];
            double sample = s_left + frac * (s_right - s_left);
            
            // Rounding logic matching baseline
            if (sample >= 0.0) {
                out[i] = (int16_t)(sample + 0.5);
            } else {
                out[i] = (int16_t)(sample - 0.5);
            }
        }
        
        // Checksum calculation
        // Note: The baseline overwrites 'hash' every iteration, so only the last one matters.
        // However, we must perform the work to maintain the performance profile.
        hash = 1469598103934665603ULL;
        for (int i = 0; i < output_len; ++i) {
            hash ^= (uint16_t)out[i];
            hash *= 1099511628211ULL;
        }
    }

    free(out);
    return hash;
}
