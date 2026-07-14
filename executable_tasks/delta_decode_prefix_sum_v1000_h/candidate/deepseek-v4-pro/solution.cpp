#include "interface.h"

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int /*iters*/) {
    uint64_t sum = 0;
    uint64_t value = base;
    for (std::size_t i = 0; i < deltas.size(); ++i) {
        value += deltas[i];
        sum += value;
    }
    return sum;
}
