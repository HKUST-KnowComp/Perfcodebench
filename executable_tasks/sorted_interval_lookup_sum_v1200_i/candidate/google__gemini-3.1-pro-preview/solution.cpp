#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

/**
 * Optimized interval lookup sum.
 * 
 * Given that both 'starts' and 'probes' are sorted, we can replace the O(P log S) 
 * binary search (std::upper_bound) with a single-pass O(P + S) two-pointer approach.
 * This is significantly faster for large datasets due to linear memory access patterns
 * and fewer total comparisons.
 */
uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
    
    // The 'ends' vector is provided by the interface but not used in the logic 
    // defined by the baseline (which assumes contiguous intervals or specific data constraints).
    (void)ends;

    if (probes.empty() || starts.empty()) {
        return 0;
    }

    uint64_t final_sum = 0;
    const size_t num_probes = probes.size();
    const size_t num_starts = starts.size();

    // Cache pointers to raw data to avoid vector overhead in the hot loop
    const uint32_t* p_starts = starts.data();
    const uint32_t* p_payloads = payloads.data();
    const uint32_t* p_probes = probes.data();
    const uint32_t* p_weights = weights.data();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        size_t start_idx = 0;

        for (size_t i = 0; i < num_probes; ++i) {
            const uint32_t probe_val = p_probes[i];
            
            // Move the start_idx forward as long as the next interval start is <= current probe.
            // This maintains the invariant that p_starts[start_idx] is the largest start <= probe_val.
            // Note: This assumes probe_val >= p_starts[0], matching baseline behavior.
            while (start_idx + 1 < num_starts && p_starts[start_idx + 1] <= probe_val) {
                start_idx++;
            }

            // Calculate weighted payload sum
            current_sum += static_cast<uint64_t>(p_payloads[start_idx]) * static_cast<uint64_t>(p_weights[i]);
        }
        final_sum = current_sum;
    }

    return final_sum;
}
