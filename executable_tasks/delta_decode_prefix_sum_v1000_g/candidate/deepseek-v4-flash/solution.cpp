#include "interface.h"

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int /*iters*/) {
    uint64_t cur = base;
    uint64_t total = 0;
    for (uint32_t d : deltas) {
        cur += d;
        total += cur;
    }
    return total;
}
