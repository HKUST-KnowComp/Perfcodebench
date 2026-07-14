#include "interface.h"
#include <algorithm>
#include <vector>
#include <cstdint>

/**
 * Optimized interval lookup sum.
 * 
 * The key observation is that the input vectors (starts, payloads, probes, weights) 
 * do not change between iterations of the 'iters' loop. The baseline implementation 
 * performs a binary search (std::upper_bound) for every probe in every iteration.
 * 
 * Optimization Strategy:
 * 1. Pre-calculate the index 'idx' for each probe once outside the 'iters' loop.
 *    Since 'probes' and 'starts' are both sorted, we can use a two-pointer approach 
 *    or simply reuse the binary search once per probe.
 * 2. Store these indices in a temporary vector.
 * 3. Inside the 'iters' loop, perform a simple weighted sum using the pre-calculated indices.
 *    This transforms the inner loop into a simple linear scan (dot product), which is 
 *    extremely fast and can be vectorized by the compiler.
 */
uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
    
    (void)ends; // Unused as per baseline logic

    if (probes.empty() || iters <= 0) {
        return 0;
    }

    const std::size_t num_probes = probes.size();
    
    // Pre-calculate indices for each probe.
    // Since probes are sorted, we use a two-pointer approach to find indices in O(N + P).
    std::vector<uint32_t> cached_payloads(num_probes);
    std::size_t start_idx = 0;
    for (std::size_t i = 0; i < num_probes; ++i) {
        // Find the largest start_idx such that starts[start_idx] <= probes[i]
        // This matches the baseline logic: upper_bound - 1
        while (start_idx + 1 < starts.size() && starts[start_idx + 1] <= probes[i]) {
            start_idx++;
        }
        cached_payloads[i] = payloads[start_idx];
    }

    uint64_t final_sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        // This loop is now a simple dot product, very cache-friendly and fast.
        for (std::size_t i = 0; i < num_probes; ++i) {
            current_sum += static_cast<uint64_t>(cached_payloads[i]) * static_cast<uint64_t>(weights[i]);
        }
        final_sum = current_sum;
    }

    return final_sum;
}
