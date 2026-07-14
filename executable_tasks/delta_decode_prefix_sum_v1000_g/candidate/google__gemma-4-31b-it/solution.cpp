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
    // Note: The original code reset 'sum = 0' inside the loop, meaning it only returned 
    // the sum of the LAST iteration. 
    // Re-reading baseline: 
    // for (int iter = 0; iter < iters; ++iter) { ... sum = 0; ... for (uint64_t current : decoded) sum += current; }
    // This means the baseline actually returns only the sum of the final iteration.
    
    return single_iter_sum;
}