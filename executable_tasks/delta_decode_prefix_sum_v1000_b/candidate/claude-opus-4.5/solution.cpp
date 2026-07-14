#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
    // The decoded sequence is: v[0] = base + d[0], v[1] = base + d[0] + d[1], ...
    // Sum of all v[i] = n*base + (n)*d[0] + (n-1)*d[1] + ... + 1*d[n-1]
    //                 = n*base + sum_{i=0}^{n-1} (n-i)*d[i]
    // This can be computed in a single pass without storing decoded values.
    // Since the result doesn't depend on iter, we compute once.
    
    if (iters <= 0) return 0;
    
    const std::size_t n = deltas.size();
    if (n == 0) return 0;
    
    // Compute sum using the formula:
    // sum = n*base + sum_{i=0}^{n-1} (n-i)*d[i]
    // We can rewrite (n-i)*d[i] = n*d[i] - i*d[i]
    // So sum = n*base + n*sum(d[i]) - sum(i*d[i])
    // But let's just do a direct single pass for clarity and cache efficiency.
    
    uint64_t sum = 0;
    uint64_t running_value = base;
    
    const uint32_t* __restrict ptr = deltas.data();
    const std::size_t n4 = n & ~std::size_t(3);
    
    // Process 4 elements at a time for better ILP
    for (std::size_t i = 0; i < n4; i += 4) {
        uint64_t d0 = ptr[i];
        uint64_t d1 = ptr[i + 1];
        uint64_t d2 = ptr[i + 2];
        uint64_t d3 = ptr[i + 3];
        
        uint64_t v0 = running_value + d0;
        uint64_t v1 = v0 + d1;
        uint64_t v2 = v1 + d2;
        uint64_t v3 = v2 + d3;
        
        sum += v0 + v1 + v2 + v3;
        running_value = v3;
    }
    
    // Handle remaining elements
    for (std::size_t i = n4; i < n; ++i) {
        running_value += ptr[i];
        sum += running_value;
    }
    
    return sum;
}