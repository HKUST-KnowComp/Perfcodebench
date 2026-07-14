#include "interface.h"
#include <vector>
#include <cstdint>

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
    const uint32_t n = keys.size();
    std::vector<uint32_t> remap(n);
    for (uint32_t i = 0; i < n; ++i) {
        remap[keys[i]] = values[i];
    }

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (uint32_t probe : probes) {
            hash = mix(hash, remap[probe]);
        }
    }
    return hash;
}