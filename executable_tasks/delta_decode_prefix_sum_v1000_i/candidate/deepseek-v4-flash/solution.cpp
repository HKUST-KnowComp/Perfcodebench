#include "interface.h"

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
    if (iters <= 0) return 0;
    uint64_t sum = 0;
    uint64_t value = static_cast<uint64_t>(base);
    for (auto delta : deltas) {
        value += static_cast<uint64_t>(delta);
        sum += value;
    }
    return sum;
}