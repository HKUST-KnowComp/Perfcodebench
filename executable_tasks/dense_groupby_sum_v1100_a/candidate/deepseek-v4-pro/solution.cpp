#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t groupby_checksum(const std::vector<uint32_t>& keys, const std::vector<uint32_t>& values, int groups, int iters) {
    if (groups <= 0) return 1469598103934665603ULL;
    std::vector<uint64_t> sums(static_cast<std::size_t>(groups), 0);
    const std::size_t n = keys.size();
    const uint32_t gmax = static_cast<uint32_t>(groups);
    for (std::size_t i = 0; i < n; ++i) {
        uint32_t key = keys[i];
        if (key < gmax) {
            sums[key] += values[i];
        }
    }
    uint64_t hash = 1469598103934665603ULL;
    for (int g = 0; g < groups; ++g) {
        uint64_t v = sums[static_cast<std::size_t>(g)];
        hash ^= v;
        hash *= 1099511628211ULL;
    }
    return hash;
}