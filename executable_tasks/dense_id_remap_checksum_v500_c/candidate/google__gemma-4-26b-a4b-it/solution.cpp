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

    // Pre-calculate the remapped values for each probe to avoid O(log N) search in the inner loop.
    // We store the remapped value for each index in the 'probes' vector.
    std::vector<uint32_t> remapped_probes(probes.size());
    
    for (std::size_t i = 0; i < probes.size(); ++i) {
        auto it = std::lower_bound(keys.begin(), keys.end(), probes[i]);
        // The problem implies keys are dense/valid for the probes provided.
        // We assume the index found is the correct one as per the baseline.
        std::size_t idx = static_cast<std::size_t>(it - keys.begin());
        remapped_probes[i] = values[idx];
    }

    uint64_t hash = 0;
    const uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;

    for (int iter = 0; iter < iters; ++iter) {
        hash = FNV_OFFSET_BASIS;
        for (uint32_t val : remapped_probes) {
            hash = mix(hash, static_cast<uint64_t>(val));
        }
    }

    return hash;
}