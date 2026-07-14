#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
    if (iters <= 0) return 0;
    
    const std::size_t n = deltas.size();
    if (n == 0) return 0;
    
    // The prefix sum of decoded values can be computed as:
    // decoded[i] = base + delta[0] + delta[1] + ... + delta[i]
    // sum of decoded = sum_{i=0}^{n-1} decoded[i]
    //                = n*base + (n)*delta[0] + (n-1)*delta[1] + ... + 1*delta[n-1]
    //                = n*base + sum_{i=0}^{n-1} (n-i)*delta[i]
    
    // Compute in a single pass:
    // running_sum accumulates delta[0] + delta[1] + ... + delta[i]
    // total accumulates the sum of all prefix values
    
    uint64_t running_sum = 0;
    uint64_t total = 0;
    
    for (std::size_t i = 0; i < n; ++i) {
        running_sum += deltas[i];
        // decoded[i] = base + running_sum
        total += static_cast<uint64_t>(base) + running_sum;
    }
    
    // Result is the same for every iteration, so multiply
    return total * static_cast<uint64_t>(iters);
}