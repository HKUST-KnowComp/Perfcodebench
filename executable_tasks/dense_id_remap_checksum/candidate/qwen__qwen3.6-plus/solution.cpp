#include "interface.h"
#include <vector>

uint64_t remap_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& probes,
    int iters) {
    
    if (iters <= 0 || probes.empty() || keys.empty()) {
        return (iters > 0) ? 1469598103934665603ULL : 0ULL;
    }

    uint32_t max_key = keys.back();
    std::vector<uint32_t> table(max_key + 1);
    for (size_t i = 0; i < keys.size(); ++i) {
        table[keys[i]] = values[i];
    }

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (uint32_t probe : probes) {
            hash ^= table[probe];
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}