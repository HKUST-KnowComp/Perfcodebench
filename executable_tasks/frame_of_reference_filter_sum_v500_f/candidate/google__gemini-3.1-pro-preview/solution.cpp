#include "interface.h"
#include <vector>
#include <cstdint>

/**
 * Optimized frame-of-reference decode and filter.
 * 
 * Performance improvements:
 * 1. Removed the intermediate 'decoded' vector allocation and resize inside the loop.
 * 2. Combined decoding (base + delta) and filtering into a single pass.
 * 3. Used a branchless range-check trick: (val - low) <= (high - low) 
 *    to replace two comparisons with one, aiding SIMD auto-vectorization.
 * 4. Ensured the inner loop is tight and uses local accumulators to minimize memory traffic.
 */
uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int iters) {
    
    const size_t n = deltas.size();
    const uint16_t* __restrict__ delta_ptr = deltas.data();
    uint64_t final_sum = 0;

    // If the range is invalid, the sum is always 0.
    if (low > high) {
        return 0;
    }

    const uint32_t range_len = high - low;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_iter_sum = 0;
        
        // The compiler can auto-vectorize this loop effectively.
        // The condition (v - low <= range_len) handles both v < low (underflow to large value) 
        // and v > high (result > range_len) in one check.
        #pragma omp simd reduction(+:current_iter_sum)
        for (size_t i = 0; i < n; ++i) {
            const uint32_t v = base + static_cast<uint32_t>(delta_ptr[i]);
            if ((v - low) <= range_len) {
                current_iter_sum += static_cast<uint64_t>(v);
            }
        }
        final_sum = current_iter_sum;
    }

    return final_sum;
}
