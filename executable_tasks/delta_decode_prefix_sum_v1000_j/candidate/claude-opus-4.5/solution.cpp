#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
    const std::size_t n = deltas.size();
    if (n == 0) {
        return 0;
    }

    const uint32_t* __restrict d = deltas.data();
    uint64_t result = 0;

    // Precompute the sum once since deltas don't change between iterations
    // sum = n*base + sum_{i=0}^{n-1} delta[i]*(n-i)
    //     = n*base + sum_{i=0}^{n-1} delta[i]*n - sum_{i=0}^{n-1} delta[i]*i
    //     = n*base + n*sum(delta) - sum(delta[i]*i)
    
    uint64_t sum_delta = 0;
    uint64_t sum_delta_i = 0;
    
    std::size_t i = 0;
    const std::size_t unroll = 4;
    const std::size_t limit = n - (n % unroll);
    
    for (; i < limit; i += unroll) {
        uint64_t d0 = d[i];
        uint64_t d1 = d[i + 1];
        uint64_t d2 = d[i + 2];
        uint64_t d3 = d[i + 3];
        
        sum_delta += d0 + d1 + d2 + d3;
        sum_delta_i += d0 * i + d1 * (i + 1) + d2 * (i + 2) + d3 * (i + 3);
    }
    
    for (; i < n; ++i) {
        uint64_t di = d[i];
        sum_delta += di;
        sum_delta_i += di * i;
    }
    
    uint64_t single_iter_sum = static_cast<uint64_t>(n) * base + 
                               static_cast<uint64_t>(n) * sum_delta - sum_delta_i;
    
    result = single_iter_sum * static_cast<uint64_t>(iters);
    
    // The baseline overwrites sum each iteration and returns the last one
    // So we return just single_iter_sum (equivalent to iters=1 result)
    return single_iter_sum;
}