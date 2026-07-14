#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
    if (deltas.empty()) return 0;

    size_t n = deltas.size();
    uint64_t total_sum = 0;

    // The prefix sum at index i is: P[i] = base + sum(deltas[0...i])
    // The total sum is: sum_{i=0}^{n-1} P[i] = n * base + sum_{i=0}^{n-1} sum_{j=0}^{i} deltas[j]
    // By rearranging the summation: sum_{j=0}^{n-1} (n - j) * deltas[j]
    
    uint64_t running_sum = 0;
    uint64_t current_val = static_cast<uint64_t>(base);
    
    for (size_t i = 0; i < n; ++i) {
        current_val += static_cast<uint64_t>(deltas[i]);
        running_sum += current_val;
    }

    // Since the result is the same for every iteration, we compute once and multiply
    return running_sum * static_cast<uint64_t>(iters);
}