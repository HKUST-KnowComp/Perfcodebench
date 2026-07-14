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
    uint64_t hash = 0;

    // Pre-calculate the split point to avoid dynamic resizing
    size_t true_count = 0;
    for (size_t i = 0; i < n; ++i) {
        if (flags[i]) {
            true_count++;
        }
    }

    for (int iter = 0; iter < iters; ++iter) {
        size_t left_idx = 0;
        size_t right_idx = true_count;

        // Single pass to partition into the pre-allocated buffer
        for (size_t i = 0; i < n; ++i) {
            if (flags[i]) {
                out[left_idx++] = values[i];
            } else {
                out[right_idx++] = values[i];
            }
        }

        // Compute hash
        hash = 1469598103934665603ULL;
        for (size_t i = 0; i < n; ++i) {
            hash = mix(hash, static_cast<uint64_t>(out[i]));
        }
    }

    return hash;
}