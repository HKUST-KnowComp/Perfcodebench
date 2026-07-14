#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
    if (deltas.empty()) return 0;

    size_t n = deltas.size();
    uint64_t total_sum = 0;

    // The prefix sum at index i is: P_i = base + sum(deltas[0...i])
    // The total sum is: sum_{i=0}^{n-1} P_i
    // Total = n * base + sum_{i=0}^{n-1} (n - i) * deltas[i]
    
    uint64_t weighted_delta_sum = 0;
    for (size_t i = 0; i < n; ++i) {
        weighted_delta_sum += static_cast<uint64_t>(n - i) * deltas[i];
    }

    uint64_t single_iter_sum = (static_cast<uint64_t>(n) * base) + weighted_delta_sum;

    for (int iter = 0; iter < iters; ++iter) {
        total_sum += single_iter_sum;
    }

    return total_sum;
}