#include "interface.h"
#include <stdint.h>

uint64_t run(const int16_t* src, int input_len, int output_len, int iters) {
    uint64_t hash = 0;
    int64_t N = (int64_t)input_len - 1;
    int64_t M = (int64_t)output_len - 1;
    int64_t half_M = M / 2;
    int64_t N_div_M = N / M;
    int64_t N_mod_M = N % M;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t h = 1469598103934665603ULL;
        int64_t left = 0;
        int64_t frac_num = 0;
        for (int i = 0; i < output_len; ++i) {
            int right = (left + 1 < input_len) ? (int)(left + 1) : (int)left;
            int64_t val = (int64_t)src[left] * (M - frac_num) + (int64_t)src[right] * frac_num;
            int16_t sample = (int16_t)((val >= 0) ? (val + half_M) / M : (val - half_M) / M);
            
            h ^= (uint16_t)sample;
            h *= 1099511628211ULL;
            
            left += N_div_M;
            frac_num += N_mod_M;
            if (frac_num >= M) {
                frac_num -= M;
                left++;
            }
        }
        hash = h;
    }
    return hash;
}
