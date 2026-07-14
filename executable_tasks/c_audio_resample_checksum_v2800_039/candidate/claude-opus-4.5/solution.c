#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

uint64_t run(const int16_t* src, int input_len, int output_len, int iters) {
    if (output_len <= 0) return 0;
    if (output_len == 1) {
        uint64_t hash = 0;
        for (int iter = 0; iter < iters; ++iter) {
            int16_t sample = src[0];
            hash = 1469598103934665603ULL;
            hash ^= (uint16_t)sample;
            hash *= 1099511628211ULL;
        }
        return hash;
    }
    
    int16_t* out = (int16_t*)malloc((size_t)output_len * sizeof(int16_t));
    if (!out) return 0;
    
    uint64_t hash = 0;
    
    /* Fixed-point arithmetic: 20 bits for fraction */
    const int FRAC_BITS = 20;
    const int64_t FRAC_ONE = 1LL << FRAC_BITS;
    const int64_t FRAC_HALF = 1LL << (FRAC_BITS - 1);
    
    /* step = (input_len - 1) / (output_len - 1) in fixed point */
    int64_t step = ((int64_t)(input_len - 1) << FRAC_BITS) / (output_len - 1);
    
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        int64_t pos_fixed = 0;
        
        for (int i = 0; i < output_len; ++i) {
            int left = (int)(pos_fixed >> FRAC_BITS);
            int64_t frac = pos_fixed & (FRAC_ONE - 1);
            
            int right = left + 1;
            if (right >= input_len) right = input_len - 1;
            
            int32_t s_left = src[left];
            int32_t s_right = src[right];
            
            /* Linear interpolation: sample = s_left * (1 - frac) + s_right * frac */
            /* = s_left + frac * (s_right - s_left) */
            int64_t diff = s_right - s_left;
            int64_t interp = ((int64_t)s_left << FRAC_BITS) + frac * diff;
            
            /* Round to nearest: add 0.5 (FRAC_HALF) for positive, subtract for negative */
            int16_t sample;
            if (interp >= 0) {
                sample = (int16_t)((interp + FRAC_HALF) >> FRAC_BITS);
            } else {
                sample = (int16_t)((interp - FRAC_HALF) >> FRAC_BITS);
            }
            
            out[i] = sample;
            
            /* Inline checksum */
            hash ^= (uint16_t)sample;
            hash *= 1099511628211ULL;
            
            pos_fixed += step;
        }
    }
    
    free(out);
    return hash;
}