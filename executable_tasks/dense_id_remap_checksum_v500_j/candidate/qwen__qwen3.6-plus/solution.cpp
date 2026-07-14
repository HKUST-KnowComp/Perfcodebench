#include "interface.h"
#include <algorithm>
#include <vector>

uint64_t remap_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& probes,
    int iters) {
    
    const size_t n = probes.size();
    std::vector<uint32_t> remapped(n);
    
    // Precompute remapped values once to avoid repeating O(log N) lookups
    for (size_t i = 0; i < n; ++i) {
        auto it = std::lower_bound(keys.begin(), keys.end(), probes[i]);
        remapped[i] = values[it - keys.begin()];
    }
    
    uint64_t hash = 0;
    const uint64_t FNV_OFFSET = 1469598103934665603ULL;
    const uint64_t FNV_PRIME = 1099511628211ULL;
    
    const uint32_t* data = remapped.data();
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t h = FNV_OFFSET;
        for (size_t i = 0; i < n; ++i) {
            h ^= data[i];
            h *= FNV_PRIME;
        }
        hash = h;
    }
    return hash;
}
