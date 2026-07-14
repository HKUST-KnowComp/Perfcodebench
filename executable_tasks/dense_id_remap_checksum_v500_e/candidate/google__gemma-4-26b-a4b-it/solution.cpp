#include "interface.h"
#include <vector>
#include <algorithm>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

} // namespace

uint64_t remap_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& probes,
    int iters) {
    
    if (probes.empty()) return 0;

    // Pre-calculate the remapped values for each probe to move the 
    // O(log N) search out of the 'iters' loop.
    std::vector<uint32_t> remapped_probes;
    remapped_probes.reserve(probes.size());

    for (uint32_t probe : probes) {
        auto it = std::lower_bound(keys.begin(), keys.end(), probe);
        // The problem implies keys are dense/valid for the probes provided
        size_t idx = std::distance(keys.begin(), it);
        remapped_probes.push_back(values[idx]);
    }

    uint64_t hash = 0;
    const uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;

    for (int iter = 0; iter < iters; ++iter) {
        hash = FNV_OFFSET_BASIS;
        for (uint32_t val : remapped_probes) {
            hash ^= val;
            hash *= 1099511628211ULL;
        }
    }

    return hash;
}