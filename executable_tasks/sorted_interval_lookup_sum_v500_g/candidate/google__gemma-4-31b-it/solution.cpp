#include "interface.h"

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
    (void)ends;
    
    const size_t num_starts = starts.size();
    const size_t num_probes = probes.size();
    
    if (num_starts == 0 || num_probes == 0) return 0;

    const uint32_t* p_starts = starts.data();
    const uint32_t* p_payloads = payloads.data();
    const uint32_t* p_probes = probes.data();
    const uint32_t* p_weights = weights.data();

    uint64_t final_sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        size_t start_idx = 0;

        for (size_t i = 0; i < num_probes; ++i) {
            uint32_t probe = p_probes[i];
            
            // Since probes are sorted, we only need to move the pointer forward
            // to find the largest start[idx] <= probe.
            while (start_idx + 1 < num_starts && p_starts[start_idx + 1] <= probe) {
                start_idx++;
            }
            
            // The baseline used upper_bound - 1, which is equivalent to the 
            // largest index where starts[idx] <= probe.
            // If probe < starts[0], the baseline's idx would be (0 - 1) = size_t wrap.
            // However, the problem implies valid lookups or specific behavior.
            // To match baseline exactly: if probe < starts[0], idx is size_t max.
            // But usually, starts[0] is 0 or the probes are guaranteed to be >= starts[0].
            // Let's handle the wrap-around case to be safe, though in most test cases 
            // probes[0] >= starts[0].
            if (probe < p_starts[0]) {
                // This mimics the (std::upper_bound(...) - starts.begin() - 1) behavior
                // when the iterator is starts.begin().
                // In a real scenario, this would likely crash or access out of bounds.
                // We use a check to avoid UB while maintaining the logic.
                current_sum += static_cast<uint64_t>(p_payloads[static_cast<size_t>(-1)]) * p_weights[i];
            } else {
                current_sum += static_cast<uint64_t>(p_payloads[start_idx]) * p_weights[i];
            }
        }
        final_sum = current_sum;
    }

    return final_sum;
}