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
    if (starts.empty() || probes.empty()) return 0;

    const uint32_t* __restrict starts_ptr = starts.data();
    const uint32_t* __restrict payloads_ptr = payloads.data();
    const uint32_t* __restrict probes_ptr = probes.data();
    const uint32_t* __restrict weights_ptr = weights.data();
    const size_t starts_size = starts.size();
    const size_t probes_size = probes.size();

    uint64_t total_sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_iter_sum = 0;
        for (size_t i = 0; i < probes_size; ++i) {
            uint32_t probe = probes_ptr[i];
            
            // Manual binary search to find the largest index j such that starts[j] <= probe
            size_t low = 0;
            size_t high = starts_size;
            while (low < high) {
                size_t mid = low + (high - low) / 2;
                if (starts_ptr[mid] <= probe) {
                    low = mid + 1;
                } else {
                    high = mid;
                }
            }

            if (low > 0) {
                size_t idx = low - 1;
                current_iter_sum += static_cast<uint64_t>(payloads_ptr[idx]) * weights_ptr[i];
            }
        }
        total_sum += current_iter_sum;
    }

    // The requirement asks for the sum over 'iters'. 
    // The baseline returns the sum of the LAST iteration if it resets sum=0 inside. 
    // Wait, the baseline code: 
    // sum = 0; for (iter...) { sum = 0; for (probes...) { sum += ... } } 
    // This means the baseline actually returns the sum of ONLY the last iteration.
    // Let's re-examine baseline: 
    // sum = 0; for (int iter = 0; iter < iters; ++iter) { sum = 0; ... }
    // This is a bug in the baseline or a very specific requirement. 
    // To preserve correctness (matching baseline), we must return the sum of the last iteration.
    
    // Re-implementing to match baseline's exact behavior (returning last iteration sum):
    uint64_t last_sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        last_sum = 0;
        for (size_t i = 0; i < probes_size; ++i) {
            uint32_t probe = probes_ptr[i];
            size_t low = 0;
            size_t high = starts_size;
            while (low < high) {
                size_t mid = low + (high - low) / 2;
                if (starts_ptr[mid] <= probe) low = mid + 1;
                else high = mid;
            }
            if (low > 0) {
                last_sum += static_cast<uint64_t>(payloads_ptr[low - 1]) * weights_ptr[i];
            }
        }
    }
    return last_sum;
}