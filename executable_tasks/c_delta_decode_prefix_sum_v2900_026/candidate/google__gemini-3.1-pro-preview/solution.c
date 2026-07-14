#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

/**
 * Optimized delta decoding with fused aggregation.
 * 
 * The baseline implementation performed two passes and used heap allocation:
 * 1. Calculate prefix sums (deltas -> values array).
 * 2. Calculate sum of (values[i] + constant).
 * 
 * Optimization strategy:
 * 1. Eliminate malloc: The 'values' array is not needed if we aggregate in a single pass.
 * 2. Loop Fusion: Combine the prefix sum calculation and the total sum accumulation.
 * 3. Mathematical simplification: sum_{i=0}^{N-1} (v_i + C) = (sum_{i=0}^{N-1} v_i) + N * C.
 * 4. Reduction of work: The 'iters' loop is preserved for benchmarking, but the inner logic
 *    is streamlined to be as cache-friendly and pipeline-efficient as possible.
 */
uint64_t run(const int32_t* deltas, size_t count, int iters) {
    if (count == 0) return 0;

    const uint64_t constant = 0x9E3779B1ULL;
    uint64_t final_total = 0;

    for (int iter = 0; iter < iters; ++iter) {
        int64_t current_prefix = 0;
        uint64_t sum_of_prefixes = 0;

        // Fusing the prefix sum and the accumulation.
        // Compilers can often vectorize this pattern or use effective pipeline scheduling.
        for (size_t i = 0; i < count; ++i) {
            current_prefix += (int64_t)deltas[i];
            sum_of_prefixes += (uint64_t)current_prefix;
        }

        // Apply the constant addition once at the end of the iteration.
        final_total = sum_of_prefixes + (uint64_t)count * constant;
    }

    return final_total;
}