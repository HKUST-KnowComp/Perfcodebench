#include "interface.h"
#include <stdint.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
    uint64_t total = 0;
    uint64_t const_term = (uint64_t)count * 0x9E3779B1ULL;
    for (int iter = 0; iter < iters; ++iter) {
        int64_t cur = 0;
        uint64_t sum_vals = 0;
        for (size_t i = 0; i < count; ++i) {
            cur += (int64_t)deltas[i];
            sum_vals += (uint64_t)cur;
        }
        total = sum_vals + const_term;
    }
    return total;
}
