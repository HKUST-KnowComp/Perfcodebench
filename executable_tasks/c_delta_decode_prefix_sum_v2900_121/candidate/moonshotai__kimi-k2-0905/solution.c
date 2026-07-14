#include "interface.h"
#include <stdint.h>
#include <stdlib.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
    uint64_t total = 0;
    for (int iter = 0; iter < iters; ++iter) {
        int64_t current = 0;
        uint64_t sum = 0;
        const int32_t* d = deltas;
        size_t i = 0;

        /* peel off groups of 4 to give the compiler something easy to vectorize */
        for (; i + 3 < count; i += 4) {
            int64_t d0 = d[i];
            int64_t d1 = d[i+1];
            int64_t d2 = d[i+2];
            int64_t d3 = d[i+3];

            current += d0;
            sum += (uint64_t)(current + 0x9E3779B1LL);

            current += d1;
            sum += (uint64_t)(current + 0x9E3779B1LL);

            current += d2;
            sum += (uint64_t)(current + 0x9E3779B1LL);

            current += d3;
            sum += (uint64_t)(current + 0x9E3779B1LL);
        }
        /* tail */
        for (; i < count; ++i) {
            current += d[i];
            sum += (uint64_t)(current + 0x9E3779B1LL);
        }
        total = sum;
    }
    return total;
}