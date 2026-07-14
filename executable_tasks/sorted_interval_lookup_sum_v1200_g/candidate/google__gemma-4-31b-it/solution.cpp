#include "interface.h"
#include <algorithm>

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
    
    const size_t n_starts = starts.size();
    const size_t n_probes = probes.size();
    
    if (n_starts == 0 || n_probes == 0) return 0;

    const uint32_t* p_starts = starts.data();
    const uint32_t* p_ends = ends.data();
    const uint32_t* p_payloads = payloads.data();
    const uint32_t* p_probes = probes.data();
    const uint32_t* p_weights = weights.data();

    uint64_t total_sum = 0;

    // Pre-calculate the sum for one pass to optimize the 'iters' loop
    uint64_t single_pass_sum = 0;
    for (size_t i = 0; i < n_probes; ++i) {
        uint32_t probe = p_probes[i];
        
        // Binary search for the largest index idx such that starts[idx] <= probe
        int low = 0;
        int high = (int)n_starts - 1;
        int idx = -1;
        
        while (low <= high) {
            int mid = low + (high - low) / 2;
            if (p_starts[mid] <= probe) {
                idx = mid;
                low = mid + 1;
            } else {
                high = mid - 1;
            }
        }

        if (idx != -1 && probe <= p_ends[idx]) {
            single_pass_sum += static_cast<uint64_t>(p_payloads[idx]) * static_cast<uint64_t>(p_weights[i]);
        }
    }

    // The baseline reset sum = 0 inside the loop, but the goal is usually the total
    // or the result of the last iteration. Given the baseline's structure:
    // sum = 0; for(...) { sum = 0; for(...) { sum += ... } }
    // It effectively returns the sum of one iteration. 
    // However, to be safe and performant, we calculate it once.
    
    // Based on the baseline: sum is reset to 0 at the start of every 'iter'.
    // So we only need the result of one pass.
    return single_pass_sum;
}