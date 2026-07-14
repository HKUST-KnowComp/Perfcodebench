#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
    if (iters <= 0 || deltas.empty()) {
        return 0;
    }

    size_t n = deltas.size();
    uint64_t single_iter_sum = 0;
    uint64_t current_value = static_cast<uint64_t>(base);

    // The sum of a single decoded sequence is:
    // Sum_{i=0}^{n-1} (base + Sum_{j=0}^{i} deltas[j])
    // This can be computed in one pass.
    for (size_t i = 0; i < n; ++i) {
        current_value += deltas[i];
        single_iter_sum += current_value;
    }

    return single_iter_sum * static_cast<uint64_t>(iters);
}