#include "interface.h"
#include <vector>

uint64_t remap_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& probes,
    int iters) {
    
    if (keys.empty() || probes.empty()) {
        return 1469598103934665603ULL;
    }

    const uint32_t min_key = keys.front();
    const uint32_t max_key = keys.back();
    
    // Direct lookup table for dense keys
    std::vector<uint32_t> table(max_key - min_key + 1);
    for (size_t i = 0; i < keys.size(); ++i) {
        table[keys[i] - min_key] = values[i];
    }

    // Precompute remapped values to avoid repeated lookups in the hot loop
    std::vector<uint32_t> remapped(probes.size());
    for (size_t i = 0; i < probes.size(); ++i) {
        remapped[i] = table[probes[i] - min_key];
    }

    uint64_t hash = 0;
    const uint64_t FNV_PRIME = 1099511628211ULL;
    const uint64_t FNV_OFFSET = 1469598103934665603ULL;

    for (int iter = 0; iter < iters; ++iter) {
        hash = FNV_OFFSET;
        for (uint32_t v : remapped) {
            hash ^= v;
            hash *= FNV_PRIME;
        }
    }
    return hash;
}