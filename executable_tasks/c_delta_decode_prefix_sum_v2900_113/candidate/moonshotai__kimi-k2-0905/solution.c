#include "interface.h"
#include <stdint.h>
#include <stdlib.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
    uint64_t total = 0;
    const int32_t* __restrict__ d = __builtin_assume_aligned(deltas, 16);
    for (int iter = 0; iter < iters; ++iter) {
        int64_t current = 0;
        uint64_t local = 0;
        size_t i = 0;
        /* 8-way unroll to hide latency and enable SIMD */
        for (; i + 7 < count; i += 8) {
            int64_t c0 = current + (int64_t)d[i+0];
            int64_t c1 = c0      + (int64_t)d[i+1];
            int64_t c2 = c1      + (int64_t)d[i+2];
            int64_t c3 = c2      + (int64_t)d[i+3];
            int64_t c4 = c3      + (int64_t)d[i+4];
            int64_t c5 = c4      + (int64_t)d[i+5];
            int64_t c6 = c5      + (int64_t)d[i+6];
            int64_t c7 = c6      + (int64_t)d[i+7];
            local += (uint64_t)(c0 + 0x9E3779B1LL);
            local += (uint64_t)(c1 + 0x9E3779B1LL);
            local += (uint64_t)(c2 + 0x9E3779B1LL);
            local += (uint64_t)(c3 + 0x9E3779B1LL);
            local += (uint64_t)(c4 + 0x9E3779B1LL);
            local += (uint64_t)(c5 + 0x9E3779B1LL);
            local += (uint64_t)(c6 + 0x9E3779B1LL);
            local += (uint64_t)(c7 + 0x9E3779B1LL);
            current = c7;
        }
        /* scalar tail */
        for (; i < count; ++i) {
            current += (int64_t)d[i];
            local   += (uint64_t)(current + 0x9E3779B1LL);
        }
        total = local;
    }
    return total;
}