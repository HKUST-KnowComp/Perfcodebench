#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
    if (iters <= 0) return 0;
    uint64_t sum = static_cast<uint64_t>(deltas.size()) * base;
    size_t n = deltas.size();
    for (size_t j = 0; j < n; ++j) {
        sum += static_cast<uint64_t>(deltas[j]) * (n - j);
    }
    return sum;
}