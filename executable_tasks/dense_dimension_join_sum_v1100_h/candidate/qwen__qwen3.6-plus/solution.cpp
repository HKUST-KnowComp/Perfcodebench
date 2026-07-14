#include "interface.h"
#include <vector>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
    
    if (dim_keys.empty() || probe_keys.empty()) return 0;

    const uint32_t offset = dim_keys[0];
    const std::size_t dim_size = dim_keys.size();
    
    std::vector<uint32_t> lookup(dim_size);
    const uint32_t* dk = dim_keys.data();
    const uint32_t* dv = dim_values.data();
    for (std::size_t i = 0; i < dim_size; ++i) {
        lookup[dk[i] - offset] = dv[i];
    }

    const uint32_t* pk = probe_keys.data();
    const uint32_t* pw = probe_weights.data();
    const std::size_t probe_size = probe_keys.size();
    
    uint64_t result = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t iter_sum = 0;
        for (std::size_t i = 0; i < probe_size; ++i) {
            iter_sum += static_cast<uint64_t>(lookup[pk[i] - offset]) * pw[i];
        }
        result = iter_sum;
    }
    return result;
}