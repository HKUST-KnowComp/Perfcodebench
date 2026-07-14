#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
    (void)iters;
    uint64_t sum = 0;
    std::size_t n = deltas.size();
    for (std::size_t i = 0; i < n; ++i) {
        sum += static_cast<uint64_t>(deltas[i]) * static_cast<uint64_t>(n - i);
    }
    sum += static_cast<uint64_t>(base) * n;
    return sum;
}
