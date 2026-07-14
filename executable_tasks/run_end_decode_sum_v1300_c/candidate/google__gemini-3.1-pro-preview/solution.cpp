#include "interface.h"

#include <vector>
#include <cstdint>

/**
 * Optimized run-end decode aggregation.
 * 
 * The original implementation explicitly expanded the run-end encoded (REE) data into a vector,
 * which is O(N) in the size of the decoded stream and involves significant memory overhead.
 * 
 * This optimized version calculates the sum directly from the REE format:
 *   Sum = sum_{i=0}^{n-1} (run_ends[i] - run_ends[i-1]) * values[i]
 * where run_ends[-1] is defined as 0.
 * 
 * This reduces the complexity to O(n) where n is the number of runs (size of the input vectors),
 * and eliminates all dynamic memory allocation.
 */
uint64_t run_end_sum(
    const std::vector<uint32_t>& run_ends,
    const std::vector<uint32_t>& values,
    int iters) {
    
    if (values.empty()) {
        return 0;
    }

    uint64_t final_sum = 0;
    const size_t num_runs = values.size();
    const uint32_t* __restrict__ re_ptr = run_ends.data();
    const uint32_t* __restrict__ val_ptr = values.data();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        uint32_t prev_end = 0;
        
        // Unrolling or SIMD could be applied here, but for a simple 
        // multiply-accumulate, the compiler's auto-vectorizer usually 
        // performs optimally given the __restrict__ hints.
        for (size_t i = 0; i < num_runs; ++i) {
            const uint32_t current_end = re_ptr[i];
            const uint32_t length = current_end - prev_end;
            current_sum += static_cast<uint64_t>(length) * static_cast<uint64_t>(val_ptr[i]);
            prev_end = current_end;
        }
        final_sum = current_sum;
    }

    return final_sum;
}