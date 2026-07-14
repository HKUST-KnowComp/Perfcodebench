#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
    const std::size_t n = deltas.size();
    const uint32_t* __restrict data = deltas.data();
    
    uint64_t result = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        // Compute sum of prefix values without storing them
        // prefix[i] = base + delta[0] + delta[1] + ... + delta[i]
        // sum of all prefix[i] = n*base + sum over i of (delta[i] * (n - i))
        // Which equals: n*base + n*sum(deltas) - sum(i * delta[i])
        // But simpler: accumulate running_value and add to sum each iteration
        
        uint64_t sum = 0;
        uint64_t value = base;
        
        // Process 4 elements at a time for better ILP
        std::size_t i = 0;
        const std::size_t unroll_limit = (n >= 4) ? (n - 3) : 0;
        
        for (; i < unroll_limit; i += 4) {
            uint64_t d0 = data[i];
            uint64_t d1 = data[i + 1];
            uint64_t d2 = data[i + 2];
            uint64_t d3 = data[i + 3];
            
            uint64_t v0 = value + d0;
            uint64_t v1 = v0 + d1;
            uint64_t v2 = v1 + d2;
            uint64_t v3 = v2 + d3;
            
            sum += v0 + v1 + v2 + v3;
            value = v3;
        }
        
        // Handle remaining elements
        for (; i < n; ++i) {
            value += data[i];
            sum += value;
        }
        
        result = sum;
    }
    
    return result;
}