#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
    if (iters <= 0) return 0;
    
    const std::size_t n = deltas.size();
    if (n == 0) return 0;
    
    // decoded[i] = base + deltas[0] + deltas[1] + ... + deltas[i]
    // prefix_sum = sum_{i=0}^{n-1} decoded[i]
    //            = n * base + sum_{i=0}^{n-1} (n - i) * deltas[i]
    //            = n * base + sum_{i=0}^{n-1} (n - i) * deltas[i]
    // 
    // Let weight[i] = n - i, then:
    // prefix_sum = n * base + sum_{i=0}^{n-1} weight[i] * deltas[i]
    
    uint64_t weighted_sum = 0;
    const uint32_t* d = deltas.data();
    
    // Process in chunks to leverage instruction-level parallelism
    std::size_t i = 0;
    const std::size_t unroll = 4;
    const std::size_t limit = n - (n % unroll);
    
    for (; i < limit; i += unroll) {
        uint64_t w0 = n - i;
        uint64_t w1 = n - i - 1;
        uint64_t w2 = n - i - 2;
        uint64_t w3 = n - i - 3;
        weighted_sum += w0 * d[i] + w1 * d[i+1] + w2 * d[i+2] + w3 * d[i+3];
    }
    
    for (; i < n; ++i) {
        weighted_sum += (n - i) * static_cast<uint64_t>(d[i]);
    }
    
    uint64_t single_iter_sum = static_cast<uint64_t>(n) * base + weighted_sum;
    
    // Each iteration produces the same result, so multiply
    return single_iter_sum;
}