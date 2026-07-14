#include "interface.h"

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
    (void)iters;
    uint64_t total = 0;
    uint64_t current = base;
    for (uint32_t d : deltas) {
        current += d;
        total += current;
    }
    return total;
}