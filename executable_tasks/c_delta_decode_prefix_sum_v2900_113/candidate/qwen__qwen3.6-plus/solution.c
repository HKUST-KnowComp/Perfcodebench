#include "interface.h"
#include <stdint.h>
#include <stddef.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
    uint64_t total = 0;
    for (int iter = 0; iter < iters; ++iter) {
        int64_t current = 0;
        uint64_t iter_sum = 0;
        for (size_t i = 0; i < count; ++i) {
            current += (int64_t)deltas[i];
            iter_sum += (uint64_t)(current + 0x9E3779B1LL);
        }
        total = iter_sum;
    }
    return total;
}