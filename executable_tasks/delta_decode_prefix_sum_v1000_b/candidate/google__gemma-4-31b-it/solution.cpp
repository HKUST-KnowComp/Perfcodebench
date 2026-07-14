#include "interface.h"

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
    if (iters <= 0) return 0;
    
    size_t n = deltas.size();
    if (n == 0) return 0;

    // The sum of the decoded sequence for one iteration is:
    // Sum_{i=0 to n-1} (base + Sum_{j=0 to i} deltas[j])
    // = n * base + Sum_{i=0 to n-1} Sum_{j=0 to i} deltas[j]
    // = n * base + Sum_{j=0 to n-1} (deltas[j] * (n - j))
    
    uint64_t single_iter_sum = (uint64_t)n * base;
    
    // Use a pointer for faster access
    const uint32_t* data = deltas.data();
    for (size_t j = 0; j < n; ++j) {
        single_iter_sum += (uint64_t)data[j] * (n - j);
    }

    // The total sum is the single iteration sum multiplied by the number of iterations
    return single_iter_sum * iters;
}