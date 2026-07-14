#include "interface.h"
#include <stdint.h>
#include <stdlib.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
    uint64_t total = 0;
    for (int iter = 0; iter < iters; ++iter) {
        const int32_t* d = deltas;
        size_t i = 0;
        int64_t current = 0;
        total = 0;

        /* unroll 4x */
        size_t limit = (count >> 2) << 2;
        for (; i < limit; i += 4) {
            current += (int64_t)d[0];
            total  += (uint64_t)(current + 0x9E3779B1LL);
            current += (int64_t)d[1];
            total  += (uint64_t)(current + 0x9E3779B1LL);
            current += (int64_t)d[2];
            total  += (uint64_t)(current + 0x9E3779B1LL);
            current += (int64_t)d[3];
            total  += (uint64_t)(current + 0x9E3779B1LL);
            d += 4;
        }
        /* tail */
        for (; i < count; ++i) {
            current += (int64_t)*d++;
            total  += (uint64_t)(current + 0x9E3779B1LL);
        }
    }
    return total;
}