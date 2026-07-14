#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
    if (iters <= 0) return 0;
    uint64_t acc = 0;
    uint64_t total = 0;
    for (uint32_t d : deltas) {
        acc += d;
        total += acc;
    }
    total += static_cast<uint64_t>(base) * deltas.size();
    return total;
}