#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t groupby_checksum(const std::vector<uint32_t>& keys, const std::vector<uint32_t>& values, int groups, int iters) {
    std::vector<uint64_t> agg(groups, 0);
    uint64_t out = 0;
    const size_t n = keys.size();
    for (int iter = 0; iter < iters; ++iter) {
        for (size_t i = 0; i < n; ++i) {
            agg[keys[i]] += static_cast<uint64_t>(values[i]);
        }
        uint64_t hash = 1469598103934665603ULL;
        for (int g = 0; g < groups; ++g) {
            uint64_t v = agg[g];
            hash ^= v;
            hash *= 1099511628211ULL;
            agg[g] = 0;
        }
        out = hash;
    }
    return out;
}
