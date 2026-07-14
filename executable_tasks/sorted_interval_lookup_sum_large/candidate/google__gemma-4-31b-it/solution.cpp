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

    const uint32_t* s_ptr = starts.data();
    const uint32_t* p_ptr = probes.data();
    const uint32_t* w_ptr = weights.data();
    const uint32_t* pay_ptr = payloads.data();
    
    const size_t num_starts = starts.size();
    const size_t num_probes = probes.size();
    uint64_t final_sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        size_t s_idx = 0;
        
        for (size_t p_idx = 0; p_idx < num_probes; ++p_idx) {
            uint32_t probe = p_ptr[p_idx];
            
            // Since probes are sorted, we only move s_idx forward
            while (s_idx < num_starts && s_ptr[s_idx] <= probe) {
                s_idx++;
            }
            
            // The matching interval is at s_idx - 1
            // Based on baseline: idx = upper_bound - 1
            if (s_idx > 0) {
                current_sum += static_cast<uint64_t>(pay_ptr[s_idx - 1]) * static_cast<uint64_t>(w_ptr[p_idx]);
            }
        }
        final_sum = current_sum;
    }
    
    return final_sum;
}