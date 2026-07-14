#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
    if (deltas.empty()) return 0;

    const size_t n = deltas.size();
    uint64_t total_sum = 0;

    // The i-th prefix value (1-indexed) is: base + deltas[0] + ... + deltas[i-1]
    // Total sum = sum_{i=1 to n} (base + sum_{j=0 to i-1} deltas[j])
    // Total sum = (n * base) + sum_{j=0 to n-1} (deltas[j] * (n - j))
    
    uint64_t weighted_delta_sum = 0;
    for (size_t i = 0; i < n; ++i) {
        weighted_delta_sum += static_cast<uint64_t>(deltas[i]) * (n - i);
    }

    total_sum = (static_cast<uint64_t>(n) * base) + weighted_delta_sum;

    // Since the result is the same for every iteration, we multiply by iters
    return total_sum * static_cast<uint64_t>(iters);
}