#include "interface.h"

#include <vector>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
    (void)iters;
    uint64_t total = 0;
    uint64_t cur = base;
    for (uint32_t d : deltas) {
        cur += d;
        total += cur;
    }
    return total;
}