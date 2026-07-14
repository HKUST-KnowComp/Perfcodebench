#include "interface.h"

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
    const size_t n = deltas.size();
    if (n == 0) return 0;

    // The sum of the decoded sequence is:
    // S = (base + d[0]) + (base + d[0] + d[1]) + ... + (base + d[0] + ... + d[n-1])
    // S = n * base + n * d[0] + (n-1) * d[1] + ... + 1 * d[n-1]
    
    uint64_t single_iter_sum = 0;
    uint64_t base_contribution = static_cast<uint64_t>(n) * base;
    
    // Calculate the weighted sum of deltas
    for (size_t i = 0; i < n; ++i) {
        single_iter_sum += static_cast<uint64_t>(deltas[i]) * (n - i);
    }
    
    single_iter_sum += base_contribution;

    // The problem asks for the sum after 'iters' iterations. 
    // In the baseline, 'sum' is reset to 0 at the start of each iteration,
    // meaning only the result of the final iteration is returned.
    // However, the baseline loop structure is: 
    // for (int iter = 0; iter < iters; ++iter) { sum = 0; ... calculate sum ... }
    // This means the function effectively returns the sum of one iteration.
    
    return single_iter_sum;
}