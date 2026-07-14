#include "interface.h"
#include <stdint.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
    const int64_t C = 0x9E3779B1LL;
    const uint64_t C_total = (uint64_t)(C * (int64_t)count);
    uint64_t total = 0;
    for (int iter = 0; iter < iters; ++iter) {
        int64_t current = 0;
        uint64_t sum_prefix = 0;
        for (size_t i = 0; i < count; ++i) {
            current += (int64_t)deltas[i];
            sum_prefix += (uint64_t)current;
        }
        total = sum_prefix + C_total;
    }
    return total;
}