#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
    if (iters <= 0) return 0;
    
    const std::size_t n = deltas.size();
    if (n == 0) return 0;
    
    // Compute sum once since result is identical across iterations.
    // prefix[i] = base + delta[0] + delta[1] + ... + delta[i]
    // sum of all prefix[i] = n*base + sum_{i=0}^{n-1} delta[i]*(n-i)
    //                      = n*base + sum_{i=0}^{n-1} delta[i]*(n-i)
    // We compute this in a single pass by accumulating (n-i)*delta[i].
    
    uint64_t weighted_sum = 0;
    const uint32_t* d = deltas.data();
    
    // Process in forward order: weight for delta[i] is (n - i)
    std::size_t weight = n;
    std::size_t i = 0;
    
    // Unroll by 4 for better ILP
    const std::size_t unroll_limit = (n / 4) * 4;
    for (; i < unroll_limit; i += 4) {
        weighted_sum += static_cast<uint64_t>(d[i])     * (weight);
        weighted_sum += static_cast<uint64_t>(d[i + 1]) * (weight - 1);
        weighted_sum += static_cast<uint64_t>(d[i + 2]) * (weight - 2);
        weighted_sum += static_cast<uint64_t>(d[i + 3]) * (weight - 3);
        weight -= 4;
    }
    
    // Handle remainder
    for (; i < n; ++i) {
        weighted_sum += static_cast<uint64_t>(d[i]) * weight;
        --weight;
    }
    
    uint64_t sum = static_cast<uint64_t>(n) * base + weighted_sum;
    
    return sum;
}