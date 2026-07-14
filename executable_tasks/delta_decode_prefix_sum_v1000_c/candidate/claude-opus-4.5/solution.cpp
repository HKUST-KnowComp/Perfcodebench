#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
    const std::size_t n = deltas.size();
    const uint32_t* __restrict data = deltas.data();
    
    // Compute the sum once using closed-form:
    // prefix[i] = base + delta[0] + delta[1] + ... + delta[i]
    // sum of all prefix[i] = n*base + sum_{i=0}^{n-1} delta[i]*(n-i)
    //                      = n*base + sum_{i=0}^{n-1} delta[i]*n - sum_{i=0}^{n-1} delta[i]*i
    // Let S = sum of deltas, W = sum of delta[i]*i
    // Result = n*base + n*S - W = n*(base + S) - W
    
    uint64_t S = 0;  // sum of deltas
    uint64_t W = 0;  // weighted sum: sum of delta[i] * i
    
    std::size_t i = 0;
    const std::size_t unroll = 4;
    const std::size_t limit = n - (n % unroll);
    
    // Unrolled loop for better ILP
    for (; i < limit; i += unroll) {
        uint64_t d0 = data[i];
        uint64_t d1 = data[i + 1];
        uint64_t d2 = data[i + 2];
        uint64_t d3 = data[i + 3];
        
        S += d0 + d1 + d2 + d3;
        W += d0 * i + d1 * (i + 1) + d2 * (i + 2) + d3 * (i + 3);
    }
    
    // Handle remainder
    for (; i < n; ++i) {
        uint64_t d = data[i];
        S += d;
        W += d * i;
    }
    
    uint64_t result = n * (static_cast<uint64_t>(base) + S) - W;
    
    // The iters parameter requires us to "run" the computation iters times.
    // Since the result is deterministic, we can just return it.
    // But we need to ensure the compiler doesn't optimize away the iters requirement.
    // Looking at the baseline, it recomputes sum each iteration and returns the last.
    // Since result is the same each time, we just return it.
    // To honor the contract and ensure timing reflects iters iterations:
    
    if (iters <= 0) return 0;
    
    // For iters > 1, we need to actually iterate to match timing expectations
    // But mathematically the result is always the same
    // The baseline overwrites sum each iteration, so final result = single iteration result
    
    // To ensure the loop isn't optimized away while keeping it fast:
    uint64_t final_result = result;
    for (int iter = 1; iter < iters; ++iter) {
        // Recompute to prevent optimization, but use the fast method
        uint64_t s = 0, w = 0;
        std::size_t j = 0;
        for (; j < limit; j += unroll) {
            uint64_t d0 = data[j];
            uint64_t d1 = data[j + 1];
            uint64_t d2 = data[j + 2];
            uint64_t d3 = data[j + 3];
            s += d0 + d1 + d2 + d3;
            w += d0 * j + d1 * (j + 1) + d2 * (j + 2) + d3 * (j + 3);
        }
        for (; j < n; ++j) {
            uint64_t d = data[j];
            s += d;
            w += d * j;
        }
        final_result = n * (static_cast<uint64_t>(base) + s) - w;
    }
    
    return final_result;
}