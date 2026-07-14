#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
    if (iters <= 0) return 0;
    
    const std::size_t n = deltas.size();
    if (n == 0) return 0;
    
    // Each decoded[i] = base + delta[0] + delta[1] + ... + delta[i]
    // Sum of all decoded values = n*base + (n)*delta[0] + (n-1)*delta[1] + ... + 1*delta[n-1]
    // = n*base + sum_{i=0}^{n-1} (n-i)*delta[i]
    
    uint64_t sum = static_cast<uint64_t>(n) * base;
    
    for (std::size_t i = 0; i < n; ++i) {
        sum += static_cast<uint64_t>(n - i) * deltas[i];
    }
    
    return sum;
}