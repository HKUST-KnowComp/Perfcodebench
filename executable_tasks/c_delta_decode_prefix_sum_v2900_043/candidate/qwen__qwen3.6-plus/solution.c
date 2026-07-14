#include "interface.h"
#include <stdint.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
    if (count == 0 || iters == 0) return 0;
    
    uint64_t per_iter_sum = 0;
    int64_t current = 0;
    for (size_t i = 0; i < count; ++i) {
        current += (int64_t)deltas[i];
        per_iter_sum += (uint64_t)(current + 0x9E3779B1LL);
    }
    
    return per_iter_sum * (uint64_t)iters;
}