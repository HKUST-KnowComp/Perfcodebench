#include "interface.h"
#include <stdint.h>

uint64_t run(const int16_t* src, int input_len, int output_len, int iters) {
    uint64_t hash = 0;
    int64_t num = input_len - 1;
    int64_t den = output_len - 1;
    if (den <= 0) return 0;

    int64_t half_den = den >> 1;
    int64_t step_int = num / den;
    int64_t step_frac = num % den;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t h = 1469598103934665603ULL;
        int left = 0;
        int64_t frac_num = 0;

        for (int i = 0; i < output_len; ++i) {
            int right = left + 1;
            if (right >= input_len) right = input_len - 1;

            int64_t val = (int64_t)src[left] * (den - frac_num) + (int64_t)src[right] * frac_num;
            int64_t rounded = (val >= 0) ? (val + half_den) / den : (val - half_den) / den;

            h ^= (uint16_t)rounded;
            h *= 1099511628211ULL;

            frac_num += step_frac;
            left += step_int;
            if (frac_num >= den) {
                frac_num -= den;
                left++;
            }
        }
        hash = h;
    }
    return hash;
}