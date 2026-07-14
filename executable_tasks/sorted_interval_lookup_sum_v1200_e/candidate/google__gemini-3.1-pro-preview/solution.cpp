#include "interface.h"
#include <algorithm>
#include <cstdint>
#include <vector>

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
    
    // The baseline ignores 'ends', so we follow that logic.
    // Since both 'starts' and 'probes' are sorted, we use a two-pointer approach.
    // This is O(N + M) per iteration instead of O(M log N).
    
    uint64_t total_sum = 0;
    const size_t num_probes = probes.size();
    const size_t num_starts = starts.size();

    if (num_probes == 0 || num_starts == 0) return 0;

    for (int iter = 0; iter < iters; ++iter) {
        total_sum = 0;
        size_t s_idx = 0;
        size_t p_idx = 0;

        while (p_idx < num_probes) {
            uint32_t current_probe = probes[p_idx];

            // Advance s_idx to the largest index such that starts[s_idx] <= current_probe
            // Because probes are sorted, s_idx only moves forward.
            while (s_idx + 1 < num_starts && starts[s_idx + 1] <= current_probe) {
                s_idx++;
            }

            // Optimization: Group all probes that fall into the same interval [starts[s_idx], starts[s_idx+1])
            // This allows us to sum their weights and perform a single multiplication.
            uint64_t weight_acc = 0;
            uint32_t current_payload = payloads[s_idx];
            
            // Find the boundary for the current interval to process probes in bulk
            uint32_t next_start = (s_idx + 1 < num_starts) ? starts[s_idx + 1] : 0xFFFFFFFF;

            // Inner loop to consume probes belonging to the same interval
            while (p_idx < num_probes && probes[p_idx] < next_start) {
                weight_acc += weights[p_idx];
                p_idx++;
            }

            total_sum += static_cast<uint64_t>(current_payload) * weight_acc;
        }
    }

    return total_sum;
}