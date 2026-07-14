#include "interface.h"
#include <cstring>
#include <vector>

uint64_t groupby_checksum(const std::vector<uint32_t>& keys, const std::vector<uint32_t>& values, int groups, int iters) {
    if (groups <= 0) return 0;
    std::vector<uint64_t> sums(static_cast<std::size_t>(groups));
    uint64_t out = 0;
    for (int iter = 0; iter < iters; ++iter) {
        std::memset(sums.data(), 0, sums.size() * sizeof(uint64_t));
        const std::size_t n = keys.size();
        for (std::size_t i = 0; i < n; ++i) {
            sums[keys[i]] += values[i];
        }
        uint64_t hash = 1469598103934665603ULL;
        for (int g = 0; g < groups; ++g) {
            hash ^= sums[static_cast<std::size_t>(g)];
            hash *= 1099511628211ULL;
        }
        out = hash;
    }
    return out;
}