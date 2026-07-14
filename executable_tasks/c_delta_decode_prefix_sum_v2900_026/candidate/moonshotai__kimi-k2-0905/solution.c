#include "interface.h"
#include <stdint.h>
#include <stdlib.h>

uint64_t run(const int32_t* restrict deltas, size_t count, int iters) {
    uint64_t total = 0;
    const uint64_t k = 0x9E3779B1ULL;
    for (int iter = 0; iter < iters; ++iter) {
        int64_t current = 0;
        uint64_t iter_sum = 0;
        for (size_t i = 0; i < count; ++i) {
            current += (int64_t)deltas[i];
            iter_sum += (uint64_t)(current + k);
        }
        total = iter_sum;
    }
    return total;
}