#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
    if (iters <= 0) return 0;

    uint64_t n = static_cast<uint64_t>(deltas.size());
    uint64_t sum_prefix = 0;
    uint64_t cur = 0;
    for (auto d : deltas) {
        cur += static_cast<uint64_t>(d);
        sum_prefix += cur;
    }
    return n * static_cast<uint64_t>(base) + sum_prefix;
}
