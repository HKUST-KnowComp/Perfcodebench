#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
    uint64_t total_sum = 0;
    const size_t n = deltas.size();
    
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        uint64_t current_val = static_cast<uint64_t>(base);
        
        // We need to calculate the sum of all prefix values.
        // If the sequence is v_1, v_2, ..., v_n where v_i = base + sum(deltas[0...i-1])
        // The total sum is n*base + sum_{i=0 to n-1} ((n-i) * deltas[i])
        // However, a simple loop is cache-friendly and efficient for modern CPUs.
        
        for (size_t i = 0; i < n; ++i) {
            current_val += static_cast<uint64_t>(deltas[i]);
            current_sum += current_val;
        }
        total_sum = current_sum;
    }
    return total_sum;
}