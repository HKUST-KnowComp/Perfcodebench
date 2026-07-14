#include "interface.h"
#include <algorithm>
#include <vector>
#include <cstdint>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

// Hand-tuned binary search for finding the first element >= target
inline uint32_t find_index(const uint32_t* keys, uint32_t size, uint32_t target) {
    uint32_t low = 0;
    uint32_t high = size;
    while (low < high) {
        uint32_t mid = low + (high - low) / 2;
        if (keys[mid] < target) {
            low = mid + 1;
        } else {
            high = mid;
        }
    }
    return low;
}

} // namespace

uint64_t remap_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& probes,
    int iters) {
    
    if (probes.empty()) return 0;

    // Pre-calculate indices to avoid repeated binary searches across iterations.
    // This is the primary optimization for iters > 1.
    std::vector<uint32_t> indices(probes.size());
    const uint32_t* keys_ptr = keys.data();
    uint32_t keys_size = static_cast<uint32_t>(keys.size());
    
    for (size_t i = 0; i < probes.size(); ++i) {
        indices[i] = find_index(keys_ptr, keys_size, probes[i]);
    }

    uint64_t final_hash = 0;
    const uint32_t* values_ptr = values.data();
    const uint32_t* idx_ptr = indices.data();
    const size_t n = indices.size();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t hash = 1469598103934665603ULL;
        
        // Unroll the hash loop to improve throughput.
        size_t i = 0;
        for (; i + 3 < n; i += 4) {
            hash = mix(hash, values_ptr[idx_ptr[i]]);
            hash = mix(hash, values_ptr[idx_ptr[i + 1]]);
            hash = mix(hash, values_ptr[idx_ptr[i + 2]]);
            hash = mix(hash, values_ptr[idx_ptr[i + 3]]);
        }
        for (; i < n; ++i) {
            hash = mix(hash, values_ptr[idx_ptr[i]]);
        }
        final_hash = hash;
    }

    return final_hash;
}
