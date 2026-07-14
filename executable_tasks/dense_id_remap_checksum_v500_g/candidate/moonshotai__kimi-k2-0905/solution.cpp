#include "interface.h"
#include <algorithm>
#include <vector>
#include <cstdint>
#include <cstring>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

}  // namespace

uint64_t remap_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& probes,
    int iters)
{
    if (keys.empty() || probes.empty() || iters <= 0) return 0;

    // Build direct-index lookup table once
    const uint32_t max_key = keys.back();
    std::vector<uint32_t> map(max_key + 1, 0);
    const size_t n = keys.size();
    for (size_t i = 0; i < n; ++i) {
        map[keys[i]] = values[i];
    }

    uint64_t hash = 1469598103934665603ULL;
    const size_t probe_count = probes.size();

    for (int iter = 0; iter < iters; ++iter) {
        // reseed hash for this iteration
        uint64_t h = hash;
        const uint32_t* p = probes.data();
        for (size_t i = 0; i < probe_count; ++i) {
            const uint32_t key = p[i];
            // keys are guaranteed to exist; map[key] is valid
            h = mix(h, map[key]);
        }
        hash = h;
    }
    return hash;
}