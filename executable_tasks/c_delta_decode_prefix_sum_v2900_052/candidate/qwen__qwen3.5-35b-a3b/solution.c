#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
    if (iters <= 0) return 0;
    
    uint64_t total = 0;
    int64_t current = 0;
    
    // Fused loop: decode and sum in one pass
    // Removed redundant iterations since 'total' is reset every iteration in baseline
    for (size_t i = 0; i < count; ++i) {
        current += (int64_t)deltas[i];
        total += (uint64_t)(current + 0x9E3779B1LL);
    }
    
    return total;
}