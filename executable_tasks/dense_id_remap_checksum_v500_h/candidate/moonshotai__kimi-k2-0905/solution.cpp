#include "interface.h"
#include <algorithm>
#include <cstring>

namespace {

inline uint64_t mix(uint64_t h, uint64_t v) {
    h ^= v;
    h *= 1099511628211ULL;
    return h;
}

} // namespace

uint64_t remap_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& probes,
    int iters)
{
    // Build direct-index lookup table once per call
    static thread_local std::vector<uint32_t> remap(1ULL << 32, 0xFFFFFFFFU);
    // Fast clear: overwrite only the slots we touched last time
    static thread_local std::vector<uint32_t> touched;
    touched.clear();
    touched.reserve(keys.size());

    for (size_t i = 0; i < keys.size(); ++i) {
        uint32_t k = keys[i];
        remap[k] = static_cast<uint32_t>(i);
        touched.push_back(k);
    }

    const uint32_t* probe_data = probes.data();
    const size_t probe_count = probes.size();
    const uint32_t* value_data = values.data();

    uint64_t hash = 1469598103934665603ULL;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t h = 1469598103934665603ULL;
        for (size_t j = 0; j < probe_count; ++j) {
            uint32_t k = probe_data[j];
            uint32_t idx = remap[k];
            h = mix(h, value_data[idx]);
        }
        hash = h;
    }

    // Restore remap slots to sentinel for next call
    for (uint32_t k : touched) remap[k] = 0xFFFFFFFFU;

    return hash;
}