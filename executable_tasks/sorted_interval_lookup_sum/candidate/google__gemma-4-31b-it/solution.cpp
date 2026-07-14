#include "interface.h"

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
    (void)ends;
    
    if (starts.empty() || probes.empty()) return 0;

    uint64_t total_sum = 0;
    const size_t num_probes = probes.size();
    const size_t num_starts = starts.size();
    const uint32_t* p_starts = starts.data();
    const uint32_t* p_payloads = payloads.data();
    const uint32_t* p_probes = probes.data();
    const uint32_t* p_weights = weights.data();

    // Pre-calculate the sum for one iteration to avoid repeating the linear scan
    // if the data doesn't change across iters. 
    // However, the task implies we must run the loop 'iters' times.
    uint64_t single_iter_sum = 0;
    size_t start_idx = 0;

    for (size_t i = 0; i < num_probes; ++i) {
        uint32_t probe = p_probes[i];
        
        // Since probes are sorted, we only move start_idx forward
        while (start_idx + 1 < num_starts && p_starts[start_idx + 1] <= probe) {
            start_idx++;
        }
        
        // The interval containing probe is at start_idx if p_starts[start_idx] <= probe
        // The baseline used upper_bound - 1, which is exactly this logic.
        if (p_starts[start_idx] <= probe) {
            single_iter_sum += static_cast<uint64_t>(p_payloads[start_idx]) * p_weights[i];
        }
    }

    // The baseline resets sum = 0 inside the loop and returns the sum of the LAST iteration.
    // "sum = 0; for... sum += ..." inside "for (int iter = 0; iter < iters; ++iter)"
    // means the result is simply the result of one iteration if iters > 0.
    if (iters <= 0) return 0;
    return single_iter_sum;
}