#include "interface.h"

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
    if (iters <= 0) return 0;
    
    size_t n = deltas.size();
    if (n == 0) return 0;

    // The sum of the prefix sum sequence is:
    // S = sum_{i=0}^{n-1} (base + sum_{j=0}^{i} deltas[j])
    // S = (n * base) + sum_{i=0}^{n-1} (sum_{j=0}^{i} deltas[j])
    // By changing the order of summation, the term deltas[j] appears (n - j) times.
    // S = (n * base) + sum_{j=0}^{n-1} (deltas[j] * (n - j))
    
    uint64_t single_iter_sum = (uint64_t)n * base;
    for (size_t j = 0; j < n; ++j) {
        single_iter_sum += (uint64_t)deltas[j] * (n - j);
    }

    // The problem asks for the sum over 'iters' iterations. 
    // Since the input deltas and base are constant across iterations,
    // we simply multiply the result of one iteration by iters.
    return single_iter_sum * iters;
}