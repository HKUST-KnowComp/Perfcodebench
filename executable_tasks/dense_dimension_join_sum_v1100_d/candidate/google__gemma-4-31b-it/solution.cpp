#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
    if (dim_keys.empty() || probe_keys.empty()) return 0;

    // Since it's a "dense dimension", we check if we can use a direct mapping array.
    // We find the max key to determine if a lookup table is feasible.
    uint32_t max_key = 0;
    for (uint32_t k : dim_keys) if (k > max_key) max_key = k;

    // Use a lookup table for O(1) access if the range is reasonable.
    // If the range is too large, we'd fall back to binary search, but for "dense" tasks,
    // a mapping array is usually the intended optimization.
    // We use a vector to avoid stack overflow for large ranges.
    std::vector<uint32_t> lookup(max_key + 1, 0);
    for (size_t i = 0; i < dim_keys.size(); ++i) {
        lookup[dim_keys[i]] = dim_values[i];
    }

    uint64_t total_sum = 0;
    const size_t probe_size = probe_keys.size();
    const uint32_t* pk = probe_keys.data();
    const uint32_t* pw = probe_weights.data();
    const uint32_t* lut = lookup.data();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t iter_sum = 0;
        for (size_t i = 0; i < probe_size; ++i) {
            iter_sum += static_cast<uint64_t>(lut[pk[i]]) * static_cast<uint64_t>(pw[i]);
        }
        total_sum = iter_sum; // The baseline resets sum = 0 each iter and returns the last one
    }

    return total_sum;
}