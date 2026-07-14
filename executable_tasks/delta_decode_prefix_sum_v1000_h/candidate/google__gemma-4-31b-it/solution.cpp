#include "interface.h"

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
    if (iters <= 0) return 0;
    
    uint64_t single_iter_sum = 0;
    uint64_t current_value = base;
    size_t n = deltas.size();
    
    // The sum of the prefix sums is: 
    // sum = (base + d0) + (base + d0 + d1) + (base + d0 + d1 + d2) ...
    // sum = n * base + n * d0 + (n-1) * d1 + (n-2) * d2 + ... + 1 * d_{n-1}
    
    single_iter_sum = (uint64_t)n * base;
    for (size_t i = 0; i < n; ++i) {
        single_iter_sum += (uint64_t)(n - i) * deltas[i];
    }
    
    return (uint64_t)iters * single_iter_sum;
}