#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
    if (deltas.empty()) return 0;

    const size_t n = deltas.size();
    uint64_t weighted_delta_sum = 0;

    // The prefix sum at index i is: P_i = base + sum_{j=0 to i} deltas[j]
    // The total sum is: sum_{i=0 to n-1} P_i = n * base + sum_{i=0 to n-1} sum_{j=0 to i} deltas[j]
    // By swapping summation order: sum_{j=0 to n-1} deltas[j] * (n - j)
    for (size_t i = 0; i < n; ++i) {
        weighted_delta_sum += static_cast<uint64_t>(deltas[i]) * (n - i);
    }

    uint64_t total_sum = (static_cast<uint64_t>(n) * base) + weighted_delta_sum;

    // Since the result is the same for every iteration, we multiply by iters
    return total_sum * static_cast<uint64_t>(iters);
}