#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
    (void)iters;
    const size_t n = deltas.size();
    uint64_t sum = static_cast<uint64_t>(n) * base;
    const uint32_t* data = deltas.data();
    for (size_t i = 0; i < n; ++i) {
        sum += static_cast<uint64_t>(n - i) * data[i];
    }
    return sum;
}
