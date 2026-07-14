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
    const uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
    uint64_t final_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_hash = FNV_OFFSET_BASIS;
        
        // First pass: process all elements where flag is true (stable partition: true comes first)
        for (size_t i = 0; i < n; ++i) {
            if (flags[i]) {
                current_hash = mix(current_hash, values[i]);
            }
        }

        // Second pass: process all elements where flag is false (stable partition: false comes second)
        for (size_t i = 0; i < n; ++i) {
            if (!flags[i]) {
                current_hash = mix(current_hash, values[i]);
            }
        }
        
        final_hash = current_hash;
    }

    return final_hash;
}