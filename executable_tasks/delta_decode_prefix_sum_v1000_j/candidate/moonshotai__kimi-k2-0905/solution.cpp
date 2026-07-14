#include "interface.h"

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
    const std::size_t n = deltas.size();
    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t value = base;
        uint64_t local = 0;
        for (std::size_t i = 0; i < n; ++i) {
            value += static_cast<uint64_t>(deltas[i]);
            local += value;
        }
        sum = local;
    }
    return sum;
}