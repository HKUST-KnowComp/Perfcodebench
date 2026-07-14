#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
    uint64_t total_sum = 0;
    const int64_t offset = 0x9E3779B1LL;

    for (int iter = 0; iter < iters; ++iter) {
        int64_t current_prefix = 0;
        uint64_t iter_sum = 0;
        
        // Fuse the prefix sum calculation and the aggregation
        // This removes the O(count) memory allocation and the second pass over the data
        for (size_t i = 0; i < count; ++i) {
            current_prefix += (int64_t)deltas[i];
            iter_sum += (uint64_t)(current_prefix + offset);
        }
        total_sum = iter_sum;
    }
    
    return total_sum;
}