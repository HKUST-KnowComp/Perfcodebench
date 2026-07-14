#include "interface.h"
#include <stdint.h>
#include <stdlib.h>

uint64_t run(const int16_t* src, int input_len, int output_len, int iters) {
    if (iters <= 0) return 0;
    if (output_len <= 0) return 1469598103934665603ULL;

    double ratio = (output_len > 1) ? (double)(input_len - 1) / (double)(output_len - 1) : 0.0;
    uint64_t hash = 1469598103934665603ULL;

    for (int i = 0; i < output_len; ++i) {
        double pos = (double)i * ratio;
        int left = (int)pos;
        int right = left + 1;
        if (right >= input_len) right = input_len - 1;

        double frac = pos - (double)left;
        double sample = (double)src[left] * (1.0 - frac) + (double)src[right] * frac;

        int16_t val;
        if (sample >= 0.0) {
            val = (int16_t)(sample + 0.5);
        } else {
            val = (int16_t)(sample - 0.5);
        }

        hash ^= (uint16_t)val;
        hash *= 1099511628211ULL;
    }
    return hash;
}