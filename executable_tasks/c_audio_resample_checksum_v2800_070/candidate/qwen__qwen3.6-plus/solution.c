#include "interface.h"
#include <stdint.h>

uint64_t run(const int16_t* src, int input_len, int output_len, int iters) {
    if (output_len <= 1) return 0;
    int64_t N = input_len - 1;
    int64_t M = output_len - 1;
    int64_t q = N / M;
    int64_t r = N % M;
    int64_t half_M = M / 2;

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t h = 1469598103934665603ULL;
        int64_t left = 0;
        int64_t rem = 0;
        for (int i = 0; i < output_len; ++i) {
            int right = left + (left < N);
            int64_t val = (int64_t)src[left] * (M - rem) + (int64_t)src[right] * rem;
            int64_t bias = (val >= 0) ? half_M : -half_M;
            int64_t rounded = (val + bias) / M;
            uint16_t sample = (uint16_t)(int16_t)rounded;
            h ^= sample;
            h *= 1099511628211ULL;

            left += q;
            rem += r;
            if (rem >= M) {
                rem -= M;
                left += 1;
            }
        }
        hash = h;
    }
    return hash;
}
