#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
    uint64_t total = 0;
    const int64_t offset = 0x9E3779B1LL;

    for (int iter = 0; iter < iters; ++iter) {
        int64_t current = 0;
        uint64_t iter_sum = 0;
        
        // The sum of prefix sums: 
        // S = sum_{i=0}^{count-1} (sum_{j=0}^{i} deltas[j])
        // S = sum_{i=0}^{count-1} (count - i) * deltas[i]
        // However, the baseline adds a constant offset to each decoded value.
        // Total = sum_{i=0}^{count-1} (prefix_sum[i] + offset)
        // Total = (sum_{i=0}^{count-1} prefix_sum[i]) + (count * offset)
        
        for (size_t i = 0; i < count; ++i) {
            current += (int64_t)deltas[i];
            iter_sum += (uint64_t)(current + offset);
        }
        total = iter_sum;
    }
    
    return total;
}