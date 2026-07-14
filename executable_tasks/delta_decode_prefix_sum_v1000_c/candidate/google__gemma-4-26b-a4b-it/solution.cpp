#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
    uint64_t total_sum = 0;
    const size_t n = deltas.size();
    const uint32_t* __restrict__ d_ptr = deltas.data();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_value = base;
        uint64_t iteration_sum = 0;
        for (size_t i = 0; i < n; ++i) {
            current_value += d_ptr[i];
            iteration_sum += current_value;
        }
        total_sum += iteration_sum;
    }

    return total_sum;
}