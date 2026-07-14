#include "interface.h"
#include <vector>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
    uint64_t current_value = base;
    uint64_t total_sum = 0;
    for (uint32_t delta : deltas) {
        current_value += delta;
        total_sum += current_value;
    }
    return total_sum;
}