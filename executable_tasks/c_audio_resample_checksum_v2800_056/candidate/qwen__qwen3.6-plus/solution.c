#include "interface.h"
#include <stdint.h>
#include <stdlib.h>

uint64_t run(const int16_t* src, int input_len, int output_len, int iters) {
    uint64_t final_hash = 0;
    int64_t N = input_len - 1;
    int64_t M = output_len - 1;
    if (M <= 0) return 0;
    int64_t half_M = M / 2;
    const uint64_t FNV_PRIME = 1099511628211ULL;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t hash = 1469598103934665603ULL;
        int64_t cur_left = 0;
        int64_t cur_rem = 0;

        for (int i = 0; i < output_len; ++i) {
            int64_t right = cur_left + 1;
            if (right >= input_len) right = input_len - 1;

            int64_t val = (int64_t)src[cur_left] * (M - cur_rem) + (int64_t)src[right] * cur_rem;
            int16_t s = (int16_t)((val + (val >= 0 ? half_M : -half_M)) / M);

            hash ^= (uint16_t)s;
            hash *= FNV_PRIME;

            cur_rem += N;
            if (cur_rem >= M) {
                cur_rem -= M;
                cur_left++;
            }
        }
        final_hash = hash;
    }
    return final_hash;
}