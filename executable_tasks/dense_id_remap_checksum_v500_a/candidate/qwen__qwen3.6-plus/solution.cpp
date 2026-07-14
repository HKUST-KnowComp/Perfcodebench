#include "interface.h"
#include <vector>
#include <algorithm>

uint64_t remap_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& probes,
    int iters) {
    
    std::vector<uint32_t> remapped(probes.size());
    
    if (!keys.empty()) {
        uint64_t range = (uint64_t)keys.back() - keys.front() + 1;
        if (range == keys.size()) {
            uint32_t offset = keys.front();
            for (size_t i = 0; i < probes.size(); ++i) {
                remapped[i] = values[probes[i] - offset];
            }
        } else {
            for (size_t i = 0; i < probes.size(); ++i) {
                auto it = std::lower_bound(keys.begin(), keys.end(), probes[i]);
                remapped[i] = values[it - keys.begin()];
            }
        }
    }

    uint64_t hash = 0;
    const uint64_t FNV_PRIME = 1099511628211ULL;
    const uint64_t FNV_OFFSET = 1469598103934665603ULL;

    for (int iter = 0; iter < iters; ++iter) {
        hash = FNV_OFFSET;
        for (uint32_t val : remapped) {
            hash ^= val;
            hash *= FNV_PRIME;
        }
    }
    return hash;
}