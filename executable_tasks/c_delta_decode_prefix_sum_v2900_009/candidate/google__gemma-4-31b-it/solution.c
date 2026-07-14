#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
    uint64_t total = 0;
    const int64_t offset = 0x9E3779B1LL;
    
    // The sum of prefix sums is: 
    // S = sum_{i=0 to count-1} (sum_{j=0 to i} deltas[j])
    // S = sum_{j=0 to count-1} (count - j) * deltas[j]
    
    for (int iter = 0; iter < iters; ++iter) {
        int64_t current_prefix_sum_total = 0;
        for (size_t i = 0; i < count; ++i) {
            current_prefix_sum_total += (int64_t)(count - i) * deltas[i];
        }
        
        // The total for one iteration is: 
        // sum_{i=0 to count-1} (prefix_sum[i] + offset)
        // = current_prefix_sum_total + (count * offset)
        
        total = (uint64_t)(current_prefix_sum_total + (int64_t)count * offset);
    }
    
    return total;
}