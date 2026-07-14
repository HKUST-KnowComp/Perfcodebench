#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
    const std::size_t n = deltas.size();
    uint64_t sum = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        // Compute sum of prefix values directly without storing decoded values.
        // prefix[i] = base + delta[0] + delta[1] + ... + delta[i]
        // sum of all prefix[i] = n * base + sum over i of (delta[i] * (n - i))
        // 
        // Equivalently, if we accumulate running_value and sum simultaneously:
        // running_value starts at base, then adds delta[i] each step
        // sum accumulates running_value each step
        
        uint64_t running_value = base;
        sum = 0;
        
        // Process in chunks for better instruction-level parallelism
        const std::size_t unroll = 4;
        const std::size_t limit = n - (n % unroll);
        
        std::size_t i = 0;
        for (; i < limit; i += unroll) {
            uint64_t v0 = running_value + deltas[i];
            uint64_t v1 = v0 + deltas[i + 1];
            uint64_t v2 = v1 + deltas[i + 2];
            uint64_t v3 = v2 + deltas[i + 3];
            
            sum += v0 + v1 + v2 + v3;
            running_value = v3;
        }
        
        // Handle remaining elements
        for (; i < n; ++i) {
            running_value += deltas[i];
            sum += running_value;
        }
    }
    
    return sum;
}