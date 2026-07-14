#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {

    // Find min and max of dim_keys (assumed dense)
    size_t ndim = dim_keys.size();
    uint32_t min_key = dim_keys[0];
    uint32_t max_key = dim_keys[0];
    for (size_t i = 1; i < ndim; ++i) {
        if (dim_keys[i] < min_key) min_key = dim_keys[i];
        if (dim_keys[i] > max_key) max_key = dim_keys[i];
    }

    // Build direct lookup: index = key - min_key
    size_t range = static_cast<size_t>(max_key - min_key) + 1;
    std::vector<uint32_t> lookup(range);
    for (size_t i = 0; i < ndim; ++i) {
        lookup[dim_keys[i] - min_key] = dim_values[i];
    }

    size_t nprobe = probe_keys.size();
    const uint32_t* kptr = probe_keys.data();
    const uint32_t* wptr = probe_weights.data();
    uint64_t sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        for (size_t i = 0; i < nprobe; ++i) {
            size_t idx = kptr[i] - min_key;
            // All probe keys are assumed to be present in the dimension
            sum += static_cast<uint64_t>(lookup[idx]) * static_cast<uint64_t>(wptr[i]);
        }
    }

    return sum;
}
