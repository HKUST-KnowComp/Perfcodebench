#include "interface.h"
#include <vector>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
    if (iters <= 0) return 0;
    uint64_t sum = 0;
    uint64_t value = base;
    for (const auto delta : deltas) {
        value += delta;
        sum += value;
    }
    return sum;
}
