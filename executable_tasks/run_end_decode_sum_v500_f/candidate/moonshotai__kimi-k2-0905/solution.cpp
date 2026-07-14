#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t run_end_sum(
    const std::vector<uint32_t>& run_ends,
    const std::vector<uint32_t>& values,
    int iters)
{
    const size_t n = values.size();
    if (n == 0) return 0;

    uint64_t total_sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t sum = 0;
        uint32_t prev = 0;

        // Process 4 runs per iteration
        size_t i = 0;
        for (; i + 3 < n; i += 4) {
            uint64_t len0 = run_ends[i + 0] - prev;
            uint64_t len1 = run_ends[i + 1] - run_ends[i + 0];
            uint64_t len2 = run_ends[i + 2] - run_ends[i + 1];
            uint64_t len3 = run_ends[i + 3] - run_ends[i + 2];

            sum += len0 * static_cast<uint64_t>(values[i + 0]);
            sum += len1 * static_cast<uint64_t>(values[i + 1]);
            sum += len2 * static_cast<uint64_t>(values[i + 2]);
            sum += len3 * static_cast<uint64_t>(values[i + 3]);

            prev = run_ends[i + 3];
        }

        // Handle remaining runs
        for (; i < n; ++i) {
            uint64_t len = run_ends[i] - prev;
            sum += len * static_cast<uint64_t>(values[i]);
            prev = run_ends[i];
        }

        total_sum = sum;
    }

    return total_sum;
}