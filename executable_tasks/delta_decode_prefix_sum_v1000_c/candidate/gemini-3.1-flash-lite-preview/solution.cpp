#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
    uint64_t total_sum = 0;
    size_t n = deltas.size();
    
    // The sum of prefix sums S_i = base + sum_{j=0}^i deltas[j]
    // Total = sum_{i=0}^{n-1} S_i = n * base + sum_{i=0}^{n-1} (n - i) * deltas[i]
    
    uint64_t weighted_delta_sum = 0;
    for (size_t i = 0; i < n; ++i) {
        weighted_delta_sum += static_cast<uint64_t>(n - i) * deltas[i];
    }
    
    total_sum = (static_cast<uint64_t>(n) * base) + weighted_delta_sum;
    
    // Since the result is the same for every iteration, we multiply by iters
    return total_sum * iters;
}