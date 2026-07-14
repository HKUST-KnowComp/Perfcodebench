#include "interface.h"

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
    if (iters <= 0) return 0;
    
    size_t n = deltas.size();
    if (n == 0) return 0;

    // The sum of the decoded sequence for one iteration is:
    // Sum_{i=0 to n-1} (base + Sum_{j=0 to i} deltas[j])
    // = n * base + Sum_{i=0 to n-1} Sum_{j=0 to i} deltas[j]
    // = n * base + Sum_{j=0 to n-1} (n - j) * deltas[j]
    
    uint64_t single_iter_sum = (uint64_t)n * base;
    uint64_t weighted_delta_sum = 0;
    
    for (size_t j = 0; j < n; ++j) {
        weighted_delta_sum += (uint64_t)(n - j) * deltas[j];
    }
    
    single_iter_sum += weighted_delta_sum;
    
    // The problem asks for the sum over 'iters' iterations. 
    // Since the deltas and base are constant across iterations, 
    // we simply multiply the result of one iteration by iters.
    return (uint64_t)iters * single_iter_sum;
}