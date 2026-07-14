#include "interface.h"

#include <cstdint>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int /*iters*/) {
    uint64_t prefix = base;
    uint64_t sum = 0;
    for (const uint32_t d : deltas) {
        prefix += d;
        sum += prefix;
    }
    return sum;
}
