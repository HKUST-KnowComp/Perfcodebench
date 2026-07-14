#include "interface.h"
#include <vector>
#include <cstring>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
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
    if (keys.empty()) {
        uint64_t h = 1469598103934665603ULL;
        for (int i = 0; i < iters; ++i) {
            for (uint32_t p : probes) {
                h = mix(h, values[0]);
            }
        }
        return h;
    }

    uint32_t max_key = keys.back();
    std::vector<uint32_t> lut(max_key + 1);
    std::size_t k = 0;
    for (uint32_t i = 0; i <= max_key; ++i) {
        lut[i] = (k < keys.size() && keys[k] == i) ? values[k++] : 0;
    }

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (uint32_t probe : probes) {
            uint32_t v = (probe <= max_key) ? lut[probe] : 0;
            hash = mix(hash, v);
        }
    }
    return hash;
}