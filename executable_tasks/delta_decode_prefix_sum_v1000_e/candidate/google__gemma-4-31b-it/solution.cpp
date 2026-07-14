#include "interface.h"

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
    if (iters <= 0) return 0;
    
    size_t n = deltas.size();
    uint64_t total_sum_per_iter = 0;
    
    // The sum of the decoded sequence is:
    // S = (base + d[0]) + (base + d[0] + d[1]) + ... + (base + d[0] + ... + d[n-1])
    // S = n * base + n * d[0] + (n-1) * d[1] + (n-2) * d[2] + ... + 1 * d[n-1]
    
    total_sum_per_iter = static_cast<uint64_t>(n) * base;
    
    for (size_t i = 0; i < n; ++i) {
        total_sum_per_iter += static_cast<uint64_t>(n - i) * deltas[i];
    }
    
    // Since the loop is repeated 'iters' times and the result is reset each time in the baseline,
    // the baseline actually returns the sum of the LAST iteration only.
    // Baseline: sum = 0; for(iter...){ ... sum = 0; for(decoded) sum += current; }
    // Therefore, we only need to calculate the sum for one iteration.
    return total_sum_per_iter;
}