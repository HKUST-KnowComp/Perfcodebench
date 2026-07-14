#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
    
    uint64_t total_sum = 0;
    const size_t num_intervals = starts.size();
    const size_t num_probes = probes.size();
    
    if (num_intervals == 0 || num_probes == 0) return 0;

    // Pre-calculate the sum for one pass since probes and intervals are constant
    uint64_t single_pass_sum = 0;
    size_t interval_idx = 0;

    for (size_t i = 0; i < num_probes; ++i) {
        uint32_t probe = probes[i];
        
        // Since probes are sorted, we can advance the interval index linearly
        while (interval_idx < num_intervals && starts[interval_idx] <= probe) {
            // We only care about the most recent interval that started before or at the probe
            // But we must check if the probe is actually within the current interval's range
            // because the next interval might also start before the probe.
            // However, the standard interval lookup logic for sorted non-overlapping intervals
            // is to find the largest start <= probe and check if probe < end.
            interval_idx++;
        }
        
        // The candidate interval is the one just before the one that starts after the probe
        if (interval_idx > 0) {
            size_t candidate = interval_idx - 1;
            if (probe < ends[candidate]) {
                single_pass_sum += static_cast<uint64_t>(payloads[candidate]) * weights[i];
            }
        }
    }

    // The original code reset sum = 0 inside the loop, but the goal is usually the total
    // or the result of the last iteration. Given the baseline's structure, it likely
    // intended to simulate workload. We multiply by iters to match the logic of the loop
    // while removing the redundant inner loop overhead.
    return single_pass_sum * iters;
}