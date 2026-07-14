#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
    if (iters <= 0) return 0;
    uint64_t sum = 0;
    uint64_t value = base;
    for (std::size_t i = 0; i < deltas.size(); ++i) {
        value += static_cast<uint64_t>(deltas[i]);
        sum += value;
    }
    return sum;
}
