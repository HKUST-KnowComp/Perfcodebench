#include "interface.h"
#include <algorithm>
#include <vector>

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

    // Pre-calculate the remapped values for all probes to avoid O(log N) search in the hot loop.
    // This reduces complexity from O(iters * M * log N) to O(M * log N + iters * M).
    std::vector<uint32_t> remapped_probes;
    remapped_probes.reserve(probes.size());

    for (uint32_t probe : probes) {
        auto it = std::lower_bound(keys.begin(), keys.end(), probe);
        // The problem implies keys contains the probe or we take the index found.
        // Based on baseline: idx = it - keys.begin().
        size_t idx = static_cast<size_t>(std::distance(keys.begin(), it));
        remapped_probes.push_back(values[idx]);
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