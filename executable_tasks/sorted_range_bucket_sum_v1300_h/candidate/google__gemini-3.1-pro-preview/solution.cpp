#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

/**
 * Optimized range bucket sum for sorted inputs.
 * Since both boundaries and probes are sorted, we can use a two-pointer approach
 * to achieve O(N + M) complexity per iteration instead of O(M log N).
 * Given the "tiny rows" hint, a linear scan is highly efficient due to 
 * cache locality and predictable branching.
 */
uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
    
    const size_t b_size = boundaries.size();
    const size_t p_size = probes.size();
    
    // Handle empty cases early
    if (p_size == 0) return 0;
    if (b_size == 0) return 0;

    const uint32_t* __restrict b_ptr = boundaries.data();
    const uint32_t* __restrict p_ptr = probes.data();
    
    uint64_t last_sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        size_t b_idx = 0;

        // Two-pointer linear scan
        for (size_t i = 0; i < p_size; ++i) {
            const uint32_t p = p_ptr[i];
            
            // Advance boundary pointer as long as boundary <= probe
            // For tiny rows, this loop is extremely fast and hardware-friendly
            while (b_idx < b_size && b_ptr[b_idx] <= p) {
                b_idx++;
            }
            
            current_sum += static_cast<uint64_t>(b_idx);
        }
        last_sum = current_sum;
    }

    return last_sum;
}
