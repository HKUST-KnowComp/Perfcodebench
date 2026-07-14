#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
    if (count == 0) return 0;

    const int64_t offset = 0x9E3779B1LL;
    const uint64_t offset_sum = (uint64_t)count * (uint64_t)offset;

    uint64_t total = 0;

    for (int iter = 0; iter < iters; ++iter) {
        int64_t sum_deltas = 0;
        int64_t weighted_sum = 0;

        size_t i = 0;
        size_t n = count;

        /* Unroll by 4 for better ILP */
        size_t limit = n & ~(size_t)3;
        for (; i < limit; i += 4) {
            int64_t d0 = (int64_t)deltas[i];
            int64_t d1 = (int64_t)deltas[i + 1];
            int64_t d2 = (int64_t)deltas[i + 2];
            int64_t d3 = (int64_t)deltas[i + 3];

            sum_deltas += d0 + d1 + d2 + d3;
            weighted_sum += (int64_t)i * d0 + (int64_t)(i + 1) * d1 + (int64_t)(i + 2) * d2 + (int64_t)(i + 3) * d3;
        }

        /* Handle remainder */
        for (; i < n; ++i) {
            int64_t d = (int64_t)deltas[i];
            sum_deltas += d;
            weighted_sum += (int64_t)i * d;
        }

        /* sum of prefix values = n * sum_deltas - weighted_sum */
        int64_t prefix_sum = (int64_t)n * sum_deltas - weighted_sum;

        total = (uint64_t)prefix_sum + offset_sum;
    }

    return total;
}