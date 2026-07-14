#include "interface.h"
#include <stdint.h>
#include <stdlib.h>

/**
 * Optimized delta decoding with fused aggregation.
 * 
 * The original logic computes:
 *   values[i] = sum(deltas[0...i])
 *   total = sum(values[i] + 0x9E3779B1LL)
 * 
 * This can be simplified to:
 *   total = (count * 0x9E3779B1LL) + sum(values[i])
 * 
 * We can compute sum(values[i]) in a single pass without an intermediate array:
 *   current_prefix_sum += deltas[i]
 *   running_total += current_prefix_sum
 */

uint64_t run(const int32_t* deltas, size_t count, int iters) {
    if (count == 0) return 0;

    const uint64_t K = 0x9E3779B1ULL;
    uint64_t final_total = 0;

    // The benchmark harness repeats the operation 'iters' times.
    // We only need the result of the last iteration.
    for (int iter = 0; iter < iters; ++iter) {
        int64_t current_prefix = 0;
        int64_t sum_of_prefixes = 0;

        size_t i = 0;
        // Unroll the loop to improve ILP (Instruction Level Parallelism)
        for (; i + 3 < count; i += 4) {
            int64_t d0 = deltas[i];
            int64_t d1 = deltas[i + 1];
            int64_t d2 = deltas[i + 2];
            int64_t d3 = deltas[i + 3];

            int64_t p0 = current_prefix + d0;
            int64_t p1 = p0 + d1;
            int64_t p2 = p1 + d2;
            int64_t p3 = p2 + d3;

            sum_of_prefixes += (p0 + p1 + p2 + p3);
            current_prefix = p3;
        }

        // Handle remaining elements
        for (; i < count; ++i) {
            current_prefix += deltas[i];
            sum_of_prefixes += current_prefix;
        }

        // total = sum(values[i] + K) = sum(values[i]) + count * K
        // Note: The cast to uint64_t of the signed sum is equivalent to the 
        // element-wise cast in the original code due to two's complement arithmetic.
        final_total = (uint64_t)sum_of_prefixes + (uint64_t)count * K;
    }

    return final_total;
}
