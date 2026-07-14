#include "interface.h"
#include <stdint.h>
#include <stdlib.h>

uint64_t run(const int16_t* src, int input_len, int output_len, int iters) {
    if (output_len <= 1) return 0;
    int64_t N = input_len - 1;
    int64_t M = output_len - 1;
    int64_t half_M = M / 2;
    uint64_t final_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t hash = 1469598103934665603ULL;
        int left = 0;
        int64_t rem = 0;
        for (int i = 0; i < output_len; ++i) {
            int right = (left < N) ? left + 1 : left;
            int64_t num = (int64_t)src[left] * (M - rem) + (int64_t)src[right] * rem;
            int64_t rounded = (num + (num >= 0 ? half_M : -half_M)) / M;
            uint16_t sample = (uint16_t)rounded;

            hash ^= sample;
            hash *= 1099511628211ULL;

            rem += N;
            if (rem >= M) {
                rem -= M;
                left++;
            }
        }
        final_hash = hash;
    }
    return final_hash;
}
