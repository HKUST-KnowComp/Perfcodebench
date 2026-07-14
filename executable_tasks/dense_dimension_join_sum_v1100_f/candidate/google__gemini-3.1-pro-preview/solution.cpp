#include "interface.h"
#include <cstdint>
#include <vector>
#include <algorithm>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
    
    if (dim_keys.empty() || probe_keys.empty()) {
        return 0;
    }

    const size_t num_probes = probe_keys.size();
    const uint32_t* __restrict__ pk = probe_keys.data();
    const uint32_t* __restrict__ pw = probe_weights.data();
    const uint32_t* __restrict__ dv = dim_values.data();
    const uint32_t* __restrict__ dk = dim_keys.data();

    uint64_t final_sum = 0;
    const uint32_t first_key = dk[0];
    const bool is_contiguous = (dk[dim_keys.size() - 1] == first_key + (uint32_t)dim_keys.size() - 1);

    if (is_contiguous) {
        for (int iter = 0; iter < iters; ++iter) {
            uint64_t current_sum = 0;
            // The compiler can auto-vectorize this loop effectively
            #pragma omp simd reduction(+:current_sum)
            for (size_t i = 0; i < num_probes; ++i) {
                const uint32_t idx = pk[i] - first_key;
                current_sum += static_cast<uint64_t>(dv[idx]) * static_cast<uint64_t>(pw[i]);
            }
            final_sum = current_sum;
        }
    } else {
        // Fallback for non-contiguous dense keys using binary search
        for (int iter = 0; iter < iters; ++iter) {
            uint64_t current_sum = 0;
            for (size_t i = 0; i < num_probes; ++i) {
                const auto it = std::lower_bound(dim_keys.begin(), dim_keys.end(), pk[i]);
                const size_t idx = static_cast<size_t>(it - dim_keys.begin());
                current_sum += static_cast<uint64_t>(dv[idx]) * static_cast<uint64_t>(pw[i]);
            }
            final_sum = current_sum;
        }
    }

    return final_sum;
}