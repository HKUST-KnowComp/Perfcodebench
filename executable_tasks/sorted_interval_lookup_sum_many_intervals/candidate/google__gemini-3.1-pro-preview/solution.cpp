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
    
    (void)ends;
    const size_t num_probes = probes.size();
    const size_t num_starts = starts.size();

    if (num_probes == 0 || num_starts == 0) {
        return 0;
    }

    uint64_t total_sum = 0;
    const uint32_t* __restrict__ p_starts = starts.data();
    const uint32_t* __restrict__ p_payloads = payloads.data();
    const uint32_t* __restrict__ p_probes = probes.data();
    const uint32_t* __restrict__ p_weights = weights.data();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_iter_sum = 0;
        size_t j = 0; // index into starts/payloads
        
        for (size_t i = 0; i < num_probes; ) {
            // Find the correct interval index j for the current probe p_probes[i]
            // Since probes are sorted, j only moves forward.
            while (j + 1 < num_starts && p_starts[j + 1] <= p_probes[i]) {
                j++;
            }

            // Find the range of probes [i, next_i) that fall into the same interval j
            size_t next_i = i + 1;
            if (j + 1 < num_starts) {
                const uint32_t next_interval_start = p_starts[j + 1];
                while (next_i < num_probes && p_probes[next_i] < next_interval_start) {
                    next_i++;
                }
            } else {
                next_i = num_probes;
            }

            // Sum weights for all probes in the current interval and multiply by payload once
            uint64_t weight_subtotal = 0;
            const uint32_t* __restrict__ current_weights = &p_weights[i];
            const size_t count = next_i - i;
            
            // This loop is simple and likely to be vectorized by the compiler
            for (size_t k = 0; k < count; ++k) {
                weight_subtotal += current_weights[k];
            }
            
            current_iter_sum += weight_subtotal * static_cast<uint64_t>(p_payloads[j]);
            i = next_i;
        }
        total_sum = current_iter_sum;
    }

    return total_sum;
}
