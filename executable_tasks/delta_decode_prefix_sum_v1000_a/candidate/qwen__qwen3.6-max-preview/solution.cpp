#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
    if (iters <= 0) return 0;
    uint64_t sum = 0;
    uint64_t val = base;
    for (uint32_t d : deltas) {
        val += d;
        sum += val;
    }
    return sum;
}