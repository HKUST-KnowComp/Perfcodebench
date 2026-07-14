#include "interface.h"
#include <stdint.h>

uint64_t run(const int16_t* src, int input_len, int output_len, int iters) {
    if (output_len <= 0) return 0;
    if (output_len == 1) {
        uint64_t hash = 1469598103934665603ULL;
        hash ^= (uint16_t)src[0];
        hash *= 1099511628211ULL;
        return hash;
    }

    int step = input_len - 1;
    int denom = output_len - 1;
    int half_denom = denom / 2;
    uint64_t final_hash = 0;
    const uint64_t FNV_INIT = 1469598103934665603ULL;
    const uint64_t FNV_PRIME = 1099511628211ULL;

    for (int iter = 0; iter < iters; ++iter) {
        int64_t rem = 0;
        int left = 0;
        uint64_t hash = FNV_INIT;

        for (int i = 0; i < output_len; ++i) {
            int right = left + 1;
            if (right >= input_len) right = input_len - 1;

            int64_t num = (int64_t)src[left] * (denom - rem) + (int64_t)src[right] * rem;
            int64_t val = (num >= 0) ? (num + half_denom) / denom : (num - half_denom) / denom;
            int16_t s = (int16_t)val;

            hash ^= (uint16_t)s;
            hash *= FNV_PRIME;

            rem += step;
            if (rem >= denom) {
                rem -= denom;
                left++;
            }
        }
        final_hash = hash;
    }
    return final_hash;
}