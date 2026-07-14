#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
    if (iters <= 0 || deltas.empty()) {
        return 0;
    }

    size_t n = deltas.size();
    uint64_t single_iter_sum = 0;
    
    // The prefix sum at index i is: base + sum_{j=0}^{i} deltas[j]
    // The total sum for one iteration is: sum_{i=0}^{n-1} (base + sum_{j=0}^{i} deltas[j])
    // This expands to: (n * base) + sum_{i=0}^{n-1} (deltas[i] * (n - i))
    
    single_iter_sum = static_cast<uint64_t>(n) * base;
    
    for (size_t i = 0; i < n; ++i) {
        single_iter_sum += static_cast<uint64_t>(deltas[i]) * (n - i);
    }

    return single_iter_sum * static_cast<uint64_t>(iters);
}