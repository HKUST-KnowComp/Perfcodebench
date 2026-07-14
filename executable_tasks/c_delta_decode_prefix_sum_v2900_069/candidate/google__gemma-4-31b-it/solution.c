#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

/**
 * The original logic computes:
 * values[i] = sum_{j=0 to i} deltas[j]
 * total = sum_{i=0 to count-1} (values[i] + C)
 * where C = 0x9E3779B1LL
 * 
 * Mathematically:
 * total = sum_{i=0 to count-1} (sum_{j=0 to i} deltas[j]) + count * C
 * total = sum_{j=0 to count-1} (deltas[j] * (count - j)) + count * C
 */

uint64_t run(const int32_t* deltas, size_t count, int iters) {
    if (count == 0) return 0;

    const int64_t C = 0x9E3779B1LL;
    uint64_t final_total = 0;

    // Precompute the sum for one iteration
    // We use int64_t for the intermediate sum to avoid overflow before the final cast to uint64_t
    int64_t single_iter_sum = 0;
    
    // To optimize, we can use the property that deltas[j] is added (count - j) times.
    // However, the original code does: total += (uint64_t)(values[i] + C).
    // This means the addition of C happens for every i.
    
    // Let's implement the fused loop to avoid the O(N) memory allocation and the second pass.
    // The original code: 
    // for (size_t i = 0; i < count; ++i) { current += deltas[i]; total += (uint64_t)(current + C); }
    
    // We can optimize this loop using SIMD or unrolling.
    
    for (int iter = 0; iter < iters; ++iter) {
        int64_t current_val = 0;
        uint64_t iter_total = 0;
        
        size_t i = 0;
        // Unroll loop to improve throughput
        for (; i + 3 < count; i += 4) {
            current_val += deltas[i];
            iter_total += (uint64_t)(current_val + C);
            current_val += deltas[i+1];
            iter_total += (uint64_t)(current_val + C);
            current_val += deltas[i+2];
            iter_total += (uint64_t)(current_val + C);
            current_val += deltas[i+3];
            iter_total += (uint64_t)(current_val + C);
        }
        for (; i < count; ++i) {
            current_val += deltas[i];
            iter_total += (uint64_t)(current_val + C);
        }
        final_total = iter_total;
    }

    return final_total;
}
