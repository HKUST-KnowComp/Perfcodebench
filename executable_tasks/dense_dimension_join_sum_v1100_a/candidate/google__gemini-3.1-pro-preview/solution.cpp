#include "interface.h"
#include <algorithm>
#include <vector>
#include <cstdint>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
    
    if (probe_keys.empty()) return 0;

    // Pre-calculate the mapping for the dense dimension.
    // Since dim_keys is sorted and dense, we can use a LUT for the range [0, max_key].
    uint32_t max_key = dim_keys.empty() ? 0 : dim_keys.back();
    
    // We use a LUT to store the index that std::lower_bound would return.
    // This is feasible because the dimension is 'dense' and 'small'.
    std::vector<uint32_t> lut;
    if (!dim_keys.empty()) {
        lut.resize(max_key + 1);
        uint32_t current_idx = 0;
        for (uint32_t k = 0; k <= max_key; ++k) {
            while (current_idx < dim_keys.size() && dim_keys[current_idx] < k) {
                current_idx++;
            }
            lut[k] = current_idx;
        }
    }

    uint64_t final_sum = 0;
    const size_t n = probe_keys.size();
    const uint32_t* p_keys = probe_keys.data();
    const uint32_t* p_weights = probe_weights.data();
    const uint32_t* d_values = dim_values.data();
    const uint32_t* lut_ptr = lut.data();
    const uint32_t dim_size = static_cast<uint32_t>(dim_keys.size());

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t local_sum = 0;
        size_t i = 0;

        // Unrolled loop for performance
        for (; i + 3 < n; i += 4) {
            uint32_t k0 = p_keys[i];
            uint32_t k1 = p_keys[i+1];
            uint32_t k2 = p_keys[i+2];
            uint32_t k3 = p_keys[i+3];

            uint32_t idx0 = (k0 <= max_key) ? lut_ptr[k0] : dim_size;
            uint32_t idx1 = (k1 <= max_key) ? lut_ptr[k1] : dim_size;
            uint32_t idx2 = (k2 <= max_key) ? lut_ptr[k2] : dim_size;
            uint32_t idx3 = (k3 <= max_key) ? lut_ptr[k3] : dim_size;

            local_sum += static_cast<uint64_t>(d_values[idx0]) * p_weights[i];
            local_sum += static_cast<uint64_t>(d_values[idx1]) * p_weights[i+1];
            local_sum += static_cast<uint64_t>(d_values[idx2]) * p_weights[i+2];
            local_sum += static_cast<uint64_t>(d_values[idx3]) * p_weights[i+3];
        }

        for (; i < n; ++i) {
            uint32_t k = p_keys[i];
            uint32_t idx = (k <= max_key) ? lut_ptr[k] : dim_size;
            local_sum += static_cast<uint64_t>(d_values[idx]) * p_weights[i];
        }
        final_sum = local_sum;
    }

    return final_sum;
}
