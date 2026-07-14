#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
    uint64_t total_sum = 0;
    const size_t n = deltas.size();
    if (n == 0) return 0;

    // Pre-fetch pointer to avoid vector overhead in the hot loop
    const uint32_t* __restrict__ d_ptr = deltas.data();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_value = static_cast<uint64_t>(base);
        uint64_t iteration_sum = 0;
        
        // Single pass: decode and accumulate simultaneously
        // This maximizes cache locality and eliminates O(N) memory allocation
        for (size_t i = 0; i < n; ++i) {
            current_value += d_ptr[i];
            iteration_sum += current_value;
        }
        total_sum += iteration_sum;
    }

    return total_sum;
}