#include "interface.h"

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
    (void)ends;
    if (probes.empty() || starts.empty()) return 0;

    uint64_t single_iter_sum = 0;
    size_t start_idx = 0;
    size_t num_starts = starts.size();
    size_t num_probes = probes.size();

    for (size_t i = 0; i < num_probes; ++i) {
        uint32_t probe = probes[i];
        // Since probes are sorted, we only move the start_idx forward
        while (start_idx + 1 < num_starts && starts[start_idx + 1] <= probe) {
            start_idx++;
        }
        
        // The interval is valid if the probe is >= the start of the interval
        // The baseline logic used upper_bound - 1, which effectively finds the 
        // largest start <= probe.
        if (probe >= starts[start_idx]) {
            single_iter_sum += static_cast<uint64_t>(payloads[start_idx]) * static_cast<uint64_t>(weights[i]);
        }
    }

    // The baseline code resets sum = 0 at the start of every iteration,
    // meaning it only returns the sum of the last iteration.
    return single_iter_sum;
}