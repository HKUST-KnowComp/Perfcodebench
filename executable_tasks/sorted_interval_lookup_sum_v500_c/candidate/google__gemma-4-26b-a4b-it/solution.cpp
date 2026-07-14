#include "interface.h"
#include <algorithm>
#include <vector>

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
    if (probes.empty() || starts.empty()) return 0;

    // Pre-calculate the mapping to avoid repeated binary searches if probes are sorted.
    // However, the task implies we must handle the probes as provided. 
    // If probes are sorted, we use two pointers. If not, we use binary search.
    // To be safe and highly performant, we check if probes are sorted.
    
    bool probes_sorted = true;
    for (size_t i = 1; i < probes.size(); ++i) {
        if (probes[i] < probes[i-1]) {
            probes_sorted = false;
            break;
        }
    }

    uint64_t total_sum = 0;
    size_t n_probes = probes.size();
    size_t n_intervals = starts.size();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_iter_sum = 0;
        if (probes_sorted) {
            size_t interval_idx = 0;
            for (size_t i = 0; i < n_probes; ++i) {
                uint32_t p = probes[i];
                // Move interval_idx to the largest index such that starts[idx] <= p
                // We need to find the interval where starts[idx] <= p < ends[idx]
                // The baseline logic used: idx = upper_bound(starts, p) - 1
                // This assumes intervals are non-overlapping and cover the range.
                
                // Optimization: Since probes are sorted, we don't reset interval_idx
                // But we must ensure we find the correct interval. 
                // The baseline logic: idx = upper_bound(starts, p) - 1
                // This is equivalent to finding the largest idx where starts[idx] <= p.
                
                while (interval_idx + 1 < n_intervals && starts[interval_idx + 1] <= p) {
                    interval_idx++;
                }
                
                // Check if the current interval actually contains the probe
                // The baseline didn't check 'ends', it just took the payload of the interval 
                // whose start is the largest value <= probe.
                // To preserve correctness with the baseline, we follow its logic exactly.
                if (starts[interval_idx] <= p) {
                    current_iter_sum += static_cast<uint64_t>(payloads[interval_idx]) * weights[i];
                }
            }
        } else {
            for (size_t i = 0; i < n_probes; ++i) {
                auto it = std::upper_bound(starts.begin(), starts.end(), probes[i]);
                if (it != starts.begin()) {
                    size_t idx = std::distance(starts.begin(), it) - 1;
                    current_iter_sum += static_cast<uint64_t>(payloads[idx]) * weights[i];
                }
            }
        }
        total_sum += current_iter_sum;
    }

    // The baseline returns the sum of the LAST iteration if it were to overwrite, 
    // but it actually accumulates 'sum = 0' inside the loop and returns 'sum'.
    // Wait, looking at baseline: 'sum = 0' is inside the loop, so it returns the sum of the LAST iteration.
    // Let's re-read: 'sum = 0;' is inside 'for (int iter = 0; iter < iters; ++iter)'.
    // So it returns the sum of the final iteration.
    
    // Re-implementing to match baseline behavior exactly:
    uint64_t final_sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        final_sum = 0;
        if (probes_sorted) {
            size_t interval_idx = 0;
            for (size_t i = 0; i < n_probes; ++i) {
                uint32_t p = probes[i];
                while (interval_idx + 1 < n_intervals && starts[interval_idx + 1] <= p) {
                    interval_idx++;
                }
                if (starts[interval_idx] <= p) {
                    final_sum += static_cast<uint64_t>(payloads[interval_idx]) * weights[i];
                }
            }
        } else {
            for (size_t i = 0; i < n_probes; ++i) {
                auto it = std::upper_bound(starts.begin(), starts.end(), probes[i]);
                if (it != starts.begin()) {
                    size_t idx = std::distance(starts.begin(), it) - 1;
                    final_sum += static_cast<uint64_t>(payloads[idx]) * weights[i];
                }
            }
        }
    }
    return final_sum;
}