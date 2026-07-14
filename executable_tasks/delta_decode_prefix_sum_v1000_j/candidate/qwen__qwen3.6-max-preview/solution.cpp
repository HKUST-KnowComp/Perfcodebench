#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
    if (iters <= 0) return 0;
    uint64_t total = static_cast<uint64_t>(deltas.size()) * base;
    size_t n = deltas.size();
    for (size_t i = 0; i < n; ++i) {
        total += static_cast<uint64_t>(deltas[i]) * static_cast<uint64_t>(n - i);
    }
    return total;
}
