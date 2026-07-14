#include "interface.h"
#include <stdint.h>
#include <stdlib.h>

/**
 * Optimized delta decoding with fused aggregation.
 * The goal is to compute the sum of (prefix_sum[i] + constant).
 * Mathematically, if P[i] = sum(deltas[0...i]), we want sum_{i=0}^{count-1} (P[i] + C).
 * This is equivalent to count * C + sum_{i=0}^{count-1} P[i].
 * sum_{i=0}^{count-1} P[i] = sum_{i=0}^{count-1} (count - i) * deltas[i].
 */
uint64_t run(const int32_t* deltas, size_t count, int iters) {
    if (count == 0) return 0;

    uint64_t final_total = 0;
    const uint64_t constant = 0x9E3779B1ULL;

    for (int iter = 0; iter < iters; ++iter) {
        int64_t current_prefix = 0;
        uint64_t current_total = 0;

        size_t i = 0;
        // Unroll loop to reduce branch overhead and improve ILP
        // We process 4 elements at a time
        for (; i + 3 < count; i += 4) {
            int64_t d0 = deltas[i];
            int64_t d1 = deltas[i+1];
            int64_t d2 = deltas[i+2];
            int64_t d3 = deltas[i+3];

            int64_t p0 = current_prefix + d0;
            int64_t p1 = p0 + d1;
            int64_t p2 = p1 + d2;
            int64_t p3 = p2 + d3;

            current_total += (uint64_t)(p0 + constant);
            current_total += (uint64_t)(p1 + constant);
            current_total += (uint64_t)(p2 + constant);
            current_total += (uint64_t)(p3 + constant);

            current_prefix = p3;
        }

        // Handle remaining elements
        for (; i < count; ++i) {
            current_prefix += (int64_t)deltas[i];
            current_total += (uint64_t)(current_prefix + constant);
        }

        final_total = current_total;
    }

    return final_total;
}