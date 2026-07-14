#include "interface.h"
#include <stdint.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
    const uint64_t k = 0x9E3779B1ULL;
    uint64_t total = 0;

    for (int iter = 0; iter < iters; ++iter) {
        int64_t current = 0;
        uint64_t acc = 0;
        const int32_t* p = deltas;
        const int32_t* end = deltas + (count & ~3ULL);

        /* 4-way unrolled loop */
        while (p != end) {
            int64_t d0 = (int64_t)p[0];
            int64_t d1 = (int64_t)p[1];
            int64_t d2 = (int64_t)p[2];
            int64_t d3 = (int64_t)p[3];

            current += d0;
            acc += (uint64_t)current + k;

            current += d1;
            acc += (uint64_t)current + k;

            current += d2;
            acc += (uint64_t)current + k;

            current += d3;
            acc += (uint64_t)current + k;

            p += 4;
        }

        /* scalar tail */
        end = deltas + count;
        while (p != end) {
            current += (int64_t)*p++;
            acc += (uint64_t)current + k;
        }

        total = acc;
    }
    return total;
}