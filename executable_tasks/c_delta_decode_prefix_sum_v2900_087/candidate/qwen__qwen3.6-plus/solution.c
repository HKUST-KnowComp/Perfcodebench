#include "interface.h"
#include <stdint.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
    (void)iters;
    if (count == 0) return 0;
    
    int64_t sum_vals = 0;
    int64_t w = (int64_t)count;
    for (size_t i = 0; i < count; ++i) {
        sum_vals += (int64_t)deltas[i] * w;
        w--;
    }
    
    return (uint64_t)sum_vals + (uint64_t)count * 0x9E3779B1ULL;
}