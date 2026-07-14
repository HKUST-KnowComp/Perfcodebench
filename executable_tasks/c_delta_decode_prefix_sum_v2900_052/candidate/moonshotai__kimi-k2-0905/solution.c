#include "interface.h"
#include <stdint.h>
#include <stdlib.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
    uint64_t total = 0;
    const uint64_t magic = 0x9E3779B1ULL;

    for (int iter = 0; iter < iters; ++iter) {
        int64_t current = 0;
        uint64_t acc = 0;

        size_t i = 0;
        /* process 4 elements per iteration */
        for (; i + 4 <= count; i += 4) {
            int64_t d0 = deltas[i];
            int64_t d1 = deltas[i+1];
            int64_t d2 = deltas[i+2];
            int64_t d3 = deltas[i+3];

            current += d0;
            acc    += (uint64_t)(current + (int64_t)magic);

            current += d1;
            acc    += (uint64_t)(current + (int64_t)magic);

            current += d2;
            acc    += (uint64_t)(current + (int64_t)magic);

            current += d3;
            acc    += (uint64_t)(current + (int64_t)magic);
        }
        /* tail */
        for (; i < count; ++i) {
            current += deltas[i];
            acc += (uint64_t)(current + (int64_t)magic);
        }
        total = acc;
    }
    return total;
}