#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
    if (iters <= 0 || deltas.empty()) {
        return 0;
    }

    size_t n = deltas.size();
    uint64_t single_pass_sum = 0;
    uint64_t current_value = static_cast<uint64_t>(base);

    // The sum of decoded values in one pass is:
    // sum_{i=0}^{n-1} (base + sum_{j=0}^{i} deltas[j])
    // This can be rewritten as:
    // (base * n) + sum_{i=0}^{n-1} (deltas[i] * (n - i))
    
    // We calculate this in a single pass to maintain O(N) complexity
    for (size_t i = 0; i < n; ++i) {
        current_value += deltas[i];
        single_pass_sum += current_value;
    }

    return single_pass_sum * static_cast<uint64_t>(iters);
}