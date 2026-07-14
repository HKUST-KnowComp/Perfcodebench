#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
    uint64_t total_sum = 0;
    const int64_t offset = 0x9E3779B1LL;

    for (int iter = 0; iter < iters; ++iter) {
        int64_t current_prefix = 0;
        int64_t iter_sum = 0;
        
        // Fuse the prefix sum calculation and the aggregation into one pass
        // to avoid memory allocation and reduce cache misses.
        for (size_t i = 0; i < count; ++i) {
            current_prefix += deltas[i];
            iter_sum += current_prefix;
        }
        
        // The original code did: total += (uint64_t)(values[i] + offset)
        // This is equivalent to: total += sum(values[i]) + count * offset
        total_sum += (uint64_t)(iter_sum + (int64_t)count * offset);
    }
    
    return total_sum;
}