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

} // namespace

uint64_t remap_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& probes,
    int iters)
{
    if (keys.empty()) {
        uint64_t hash = 1469598103934665603ULL;
        for (int iter = 0; iter < iters; ++iter) {
            for (uint32_t p : probes) {
                hash = mix(hash, values[0]);
            }
        }
        return hash;
    }

    const uint32_t max_key = keys.back();
    std::vector<uint32_t> map(max_key + 1, 0);

    for (size_t i = 0; i < keys.size(); ++i) {
        map[keys[i]] = static_cast<uint32_t>(i);
    }

    uint64_t hash = 1469598103934665603ULL;
    for (int iter = 0; iter < iters; ++iter) {
        for (uint32_t p : probes) {
            uint32_t idx = (p <= max_key) ? map[p] : 0;
            hash = mix(hash, values[idx]);
        }
    }
    return hash;
}