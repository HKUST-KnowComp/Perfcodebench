#include "interface.h"

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
    uint64_t final_sum = 0;
    const std::size_t n = deltas.size();
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_value = base;
        uint64_t iter_sum = 0;
        for (std::size_t i = 0; i < n; ++i) {
            current_value += deltas[i];
            iter_sum += current_value;
        }
        final_sum = iter_sum;
    }
    return final_sum;
}