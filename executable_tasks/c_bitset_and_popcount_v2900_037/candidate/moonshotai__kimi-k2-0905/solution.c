#include "interface.h"
#include <stdint.h>

uint64_t run(const uint64_t* left, const uint64_t* right, size_t word_count, int iters) {
    const uint64_t m1  = 0x5555555555555555ULL;
    const uint64_t m3  = 0x3333333333333333ULL;
    const uint64_t m0  = 0x0F0F0F0F0F0F0F0FULL;
    const uint64_t h01 = 0x0101010101010101ULL;

    uint64_t total = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t cnt = 0;
        for (size_t i = 0; i < word_count; ++i) {
            uint64_t w = left[i] & right[i];
            w = w - ((w >> 1) & m1);
            w = (w & m3) + ((w >> 2) & m3);
            w = (w + (w >> 4)) & m0;
            cnt += (w * h01) >> 56;
        }
        total = cnt;
    }
    return total;
}