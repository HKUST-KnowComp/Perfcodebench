#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

uint64_t run(const int16_t* src, int input_len, int output_len, int iters) {
    if (output_len <= 1) {
        uint64_t h = 1469598103934665603ULL;
        int16_t s = (input_len > 0) ? src[0] : 0;
        h ^= (uint16_t)s;
        h *= 1099511628211ULL;
        return h;
    }

    int num = input_len - 1;
    int den = output_len - 1;
    int half_den = den / 2;
    uint64_t hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t h = 1469598103934665603ULL;
        int left = 0;
        int rem = 0;

        for (int i = 0; i < output_len; ++i) {
            int right = (left < input_len - 1) ? left + 1 : left;
            int64_t val = (int64_t)src[left] * (den - rem) + (int64_t)src[right] * rem;
            int16_t sample = (int16_t)((val >= 0) ? (val + half_den) / den : (val - half_den) / den);

            h ^= (uint16_t)sample;
            h *= 1099511628211ULL;

            rem += num;
            if (rem >= den) {
                rem -= den;
                left++;
            }
        }
        hash = h;
    }
    return hash;
}