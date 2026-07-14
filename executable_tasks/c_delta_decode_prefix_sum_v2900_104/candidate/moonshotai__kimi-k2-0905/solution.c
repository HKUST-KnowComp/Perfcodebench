#include "interface.h"
#include <stdint.h>
#include <stdlib.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
    uint64_t total = 0;
    for (int iter = 0; iter < iters; ++iter) {
        int64_t current = 0;
        uint64_t local = 0;
        const int32_t* d = deltas;
        size_t i = 0;
        /* peel remainder to allow vectorization */
        for (; i < count; ++i) {
            current += (int64_t)*d++;
            local  += (uint64_t)(current + 0x9E3779B1LL);
        }
        total = local;
    }
    return total;
}