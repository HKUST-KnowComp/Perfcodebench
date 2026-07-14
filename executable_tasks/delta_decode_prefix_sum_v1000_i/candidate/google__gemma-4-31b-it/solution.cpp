#include "interface.h"

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
    const size_t n = deltas.size();
    if (n == 0) return 0;

    uint64_t single_iter_sum = 0;
    // The sum of the prefix sum sequence is:
    // S = (base + d[0]) + (base + d[0] + d[1]) + ... + (base + d[0] + ... + d[n-1])
    // S = n * base + n * d[0] + (n-1) * d[1] + ... + 1 * d[n-1]
    
    single_iter_sum = static_cast<uint64_t>(n) * base;
    
    for (size_t i = 0; i < n; ++i) {
        single_iter_sum += static_cast<uint64_t>(deltas[i]) * (n - i);
    }

    // The problem asks for the sum over 'iters' iterations. 
    // Since the input deltas and base are constant across iterations,
    // we simply multiply the result of one iteration by iters.
    return single_iter_sum * iters;
}