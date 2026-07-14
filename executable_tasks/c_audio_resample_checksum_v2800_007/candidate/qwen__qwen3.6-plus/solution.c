#include "interface.h"
#include <stdint.h>
#include <stdlib.h>

uint64_t run(const int16_t* src, int input_len, int output_len, int iters) {
    uint64_t hash = 0;
    const double scale = (double)(input_len - 1) / (double)(output_len - 1);
    const uint64_t FNV_OFFSET = 1469598103934665603ULL;
    const uint64_t FNV_MUL = 1099511628211ULL;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t h = FNV_OFFSET;
        for (int i = 0; i < output_len; ++i) {
            double pos = (double)i * scale;
            int left = (int)pos;
            int right = left + 1;
            if (right >= input_len) right = input_len - 1;
            double frac = pos - (double)left;
            double sample = (double)src[left] + frac * ((double)src[right] - (double)src[left]);
            int16_t val = (int16_t)(sample + (sample >= 0.0 ? 0.5 : -0.5));

            h ^= (uint16_t)val;
            h *= FNV_MUL;
        }
        hash = h;
    }
    return hash;
}