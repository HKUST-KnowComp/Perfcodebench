#include "interface.h"
#include <stdint.h>

uint64_t run(const int16_t* src, int input_len, int output_len, int iters) {
    if (output_len <= 1) {
        uint64_t h = 1469598103934665603ULL;
        int16_t val = (input_len > 0) ? src[0] : 0;
        h ^= (uint16_t)val;
        h *= 1099511628211ULL;
        return h;
    }

    uint64_t hash = 0;
    const int step = input_len - 1;
    const int den = output_len - 1;
    const int half_den = den / 2;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t h = 1469598103934665603ULL;
        int64_t cur = 0;
        for (int i = 0; i < output_len; ++i) {
            int left = (int)(cur / den);
            int frac_num = (int)(cur % den);
            int right = left + 1;
            if (right >= input_len) right = input_len - 1;

            int64_t num = (int64_t)src[left] * (den - frac_num) + (int64_t)src[right] * frac_num;
            int16_t val = (int16_t)((num >= 0) ? (num + half_den) / den : (num - half_den) / den);

            h ^= (uint16_t)val;
            h *= 1099511628211ULL;

            cur += step;
        }
        hash = h;
    }
    return hash;
}