#include "interface.h"

#include <stdint.h>
#include <stddef.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
    if (count == 0) return 0;

    uint64_t final_total = 0;
    const int64_t offset = 0x9E3779B1LL;
    
    // Precompute the constant part of the sum: count * offset
    uint64_t constant_sum = (uint64_t)count * (uint64_t)offset;

    for (int iter = 0; iter < iters; ++iter) {
        int64_t prefix_sum_total = 0;
        
        // The sum of prefix sums is: 
        // S = d[0] + (d[0]+d[1]) + (d[0]+d[1]+d[2]) ...
        // S = count*d[0] + (count-1)*d[1] + (count-2)*d[2] ...
        // This allows us to compute the total in one pass without an array.
        for (size_t i = 0; i < count; ++i) {
            prefix_sum_total += (int64_t)(count - i) * deltas[i];
        }
        
        final_total = (uint64_t)prefix_sum_total + constant_sum;
    }
    
    return final_total;
}