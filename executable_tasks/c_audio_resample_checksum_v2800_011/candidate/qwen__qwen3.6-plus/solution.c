#include "interface.h"
#include <stdint.h>
#include <stdlib.h>

uint64_t run(const int16_t* src, int input_len, int output_len, int iters) {
    if (output_len < 2) return 0;
    int N = input_len - 1;
    int M = output_len - 1;
    uint64_t hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t h = 1469598103934665603ULL;
        int left = 0;
        int64_t rem = 0;
        for (int i = 0; i < output_len; ++i) {
            int right = left + 1;
            if (right >= input_len) right = input_len - 1;

            int64_t diff = src[right] - src[left];
            int64_t val = (int64_t)src[left] * M + diff * rem;
            int16_t out_val = (int16_t)((val + (val >= 0 ? M/2 : -M/2)) / M);

            h ^= (uint16_t)out_val;
            h *= 1099511628211ULL;

            rem += N;
            if (rem >= M) {
                rem -= M;
                left++;
            }
        }
        hash = h;
    }
    return hash;
}