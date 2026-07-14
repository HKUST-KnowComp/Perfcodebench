#include "interface.h"
#include <stdint.h>
#include <stdlib.h>

/**
 * Optimized delta decoding with fused aggregation.
 * 
 * The baseline calculates:
 *   v[i] = sum(deltas[0...i])
 *   total = sum(v[i] + K)
 * 
 * Mathematically, sum_{i=0}^{n-1} (v[i] + K) is:
 *   (n * K) + sum_{i=0}^{n-1} v[i]
 * 
 * Expanding the prefix sums:
 *   v[0] = d[0]
 *   v[1] = d[0] + d[1]
 *   v[2] = d[0] + d[1] + d[2]
 *   ...
 *   v[n-1] = d[0] + d[1] + ... + d[n-1]
 * 
 * The total sum of v[i] is:
 *   d[0] appears n times
 *   d[1] appears n-1 times
 *   ...
 *   d[i] appears (n-i) times
 * 
 * So, total = (n * K) + sum_{i=0}^{n-1} (n-i) * d[i]
 */

uint64_t run(const int32_t* deltas, size_t count, int iters) {
    if (count == 0) return 0;

    const uint64_t K = 0x9E3779B1LL;
    uint64_t result = 0;

    // We execute the logic 'iters' times as required by the benchmark harness.
    // The mathematical reduction allows us to compute the result in a single pass.
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t sum_v = 0;
        // Use a signed multiplier to handle the (count - i) logic.
        // We use a loop that the compiler can easily unroll or vectorize.
        for (size_t i = 0; i < count; ++i) {
            // (count - i) is the number of times deltas[i] contributes to the prefix sum total.
            int64_t weight = (int64_t)(count - i);
            sum_v += (uint64_t)(weight * (int64_t)deltas[i]);
        }
        
        // Add the constant offset part: count * 0x9E3779B1LL
        result = (uint64_t)count * K + sum_v;
    }

    return result;
}