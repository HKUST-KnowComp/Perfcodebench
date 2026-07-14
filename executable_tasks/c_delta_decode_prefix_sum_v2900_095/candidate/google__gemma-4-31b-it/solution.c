#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
    uint64_t total_sum = 0;
    const int64_t offset = 0x9E3779B1LL;

    for (int iter = 0; iter < iters; ++iter) {
        int64_t current_val = 0;
        uint64_t iter_sum = 0;
        
        size_t i = 0;
        // Unroll loop to improve throughput
        for (; i + 3 < count; i += 4) {
            current_val += deltas[i];
            iter_sum += (uint64_t)(current_val + offset);
            current_val += deltas[i+1];
            iter_sum += (uint64_t)(current_val + offset);
            current_val += deltas[i+2];
            iter_sum += (uint64_t)(current_val + offset);
            current_val += deltas[i+3];
            iter_sum += (uint64_t)(current_val + offset);
        }
        
        for (; i < count; ++i) {
            current_val += deltas[i];
            iter_sum += (uint64_t)(current_val + offset);
        }
        total_sum = iter_sum;
    }
    
    return total_sum;
}