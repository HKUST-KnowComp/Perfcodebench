#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
    (void)iters; // unused parameter, the original code only returns the last iteration's sum
    uint64_t sum = 0;
    uint64_t prefix = static_cast<uint64_t>(base);
    for (uint32_t delta : deltas) {
        prefix += static_cast<uint64_t>(delta);
        sum += prefix;
    }
    return sum;
}