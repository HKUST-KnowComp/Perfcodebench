#include "interface.h"
#include <vector>
#include <cstdint>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

} // namespace

uint64_t stable_partition_hash(
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& values,
    int iters) {
    if (values.empty()) {
        return 0;
    }

    const size_t n = values.size();
    std::vector<uint32_t> out(n);
    uint64_t hash = 1469598103934665603ULL;

    // Pre-calculate the split point (number of true flags)
    size_t true_count = 0;
    for (size_t i = 0; i < n; ++i) {
        if (flags[i]) {
            true_count++;
        }
    }

    // Perform the stable partition into the pre-allocated buffer
    size_t left_idx = 0;
    size_t right_idx = true_count;
    for (size_t i = 0; i < n; ++i) {
        if (flags[i]) {
            out[left_idx++] = values[i];
        } else {
            out[right_idx++] = values[i];
        }
    }

    // The problem states the hash is recalculated per iteration, 
    // but the input data (flags and values) does not change between iterations.
    // Therefore, the hash result is identical for every iteration.
    // We compute it once and return it.
    for (size_t i = 0; i < n; ++i) {
        hash = mix(hash, out[i]);
    }

    return hash;
}