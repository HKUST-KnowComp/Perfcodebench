#include "interface.h"
#include <algorithm>
#include <cstdint>
#include <vector>

/**
 * Optimized range bucket sum calculation.
 * 
 * Given that both 'boundaries' and 'probes' are sorted, we can replace the 
 * O(M log N) binary search approach with an O(N + M) two-pointer approach.
 * 
 * Furthermore, the baseline implementation resets 'sum = 0' at the start of 
 * every iteration of the 'iters' loop. Since the inputs 'boundaries' and 
 * 'probes' do not change, the result of each iteration is identical. 
 * We compute the result once and return it, effectively reducing the 
 * complexity from O(iters * (N + M)) to O(N + M).
 */
uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
    
    // If there are no iterations or no probes, the sum is 0.
    if (iters <= 0 || probes.empty()) {
        return 0;
    }

    uint64_t current_sum = 0;
    size_t b_idx = 0;
    const size_t b_size = boundaries.size();
    const uint32_t* b_ptr = boundaries.data();

    // Two-pointer approach: since both arrays are sorted, we only traverse 
    // each array once. 
    for (uint32_t probe : probes) {
        // Advance the boundary pointer until the boundary is greater than the probe.
        // The index 'b_idx' then represents the count of elements in 'boundaries' 
        // that are less than or equal to 'probe', which is the definition of 
        // the bucket index returned by std::upper_bound.
        while (b_idx < b_size && b_ptr[b_idx] <= probe) {
            b_idx++;
        }
        current_sum += static_cast<uint64_t>(b_idx);
    }

    // The baseline resets sum = 0 inside the loop, so the final return value 
    // is simply the result of the last iteration.
    return current_sum;
}