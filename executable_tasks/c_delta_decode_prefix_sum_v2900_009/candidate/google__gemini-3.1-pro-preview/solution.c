#include "interface.h"
#include <stdint.h>
#include <stdlib.h>

/**
 * Optimized delta decoding with fused aggregation.
 * 
 * The baseline computes:
 *   v[i] = sum_{j=0 to i} deltas[j]
 *   total = sum_{i=0 to count-1} (v[i] + 0x9E3779B1LL)
 * 
 * This can be mathematically simplified to avoid the intermediate array and the second pass:
 *   total = [sum_{i=0 to count-1} v[i]] + (count * 0x9E3779B1LL)
 * 
 * Expanding the prefix sum:
 *   v[0] = d[0]
 *   v[1] = d[0] + d[1]
 *   v[2] = d[0] + d[1] + d[2]
 *   ...
 *   sum(v[i]) = d[0]*count + d[1]*(count-1) + d[2]*(count-2) + ... + d[count-1]*1
 * 
 * This transformation allows a single-pass O(1) space complexity solution.
 */
uint64_t run(const int32_t* deltas, size_t count, int iters) {
    if (count == 0) return 0;

    uint64_t final_total = 0;
    const uint64_t constant_offset = 0x9E3779B1ULL;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t weighted_sum = 0;
        
        // Use a signed multiplier to handle the (count - i) term.
        // The compiler can easily vectorize this loop.
        for (size_t i = 0; i < count; ++i) {
            int64_t weight = (int64_t)(count - i);
            int64_t delta = (int64_t)deltas[i];
            // We cast to uint64_t to match the 2's complement behavior of the baseline
            weighted_sum += (uint64_t)(weight * delta);
        }

        // Add the constant offset accumulated over 'count' elements.
        final_total = weighted_sum + (uint64_t)count * constant_offset;
    }

    return final_total;
}
