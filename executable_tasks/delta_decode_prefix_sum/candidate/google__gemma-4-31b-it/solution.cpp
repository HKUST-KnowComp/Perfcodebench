#include "interface.h"

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
    if (iters <= 0) return 0;
    
    size_t n = deltas.size();
    uint64_t single_iter_sum = 0;
    
    // The sum of the prefix sequence is:
    // Sum = base + (base + d0) + (base + d0 + d1) + ... + (base + d0 + ... + d_{n-1})
    // Sum = n * base + n*d0 + (n-1)*d1 + (n-2)*d2 + ... + 1*d_{n-1}
    
    single_iter_sum = static_cast<uint64_t>(n) * base;
    
    for (size_t i = 0; i < n; ++i) {
        single_iter_sum += static_cast<uint64_t>(n - i) * deltas[i];
    }
    
    // The problem asks for the sum over 'iters' iterations.
    // Since the input deltas and base are constant across iterations,
    // we simply multiply the result of one iteration by iters.
    return single_iter_sum * iters;
}