#include "interface.h"

#include <algorithm>
#include <vector>

uint64_t groupby_checksum(const std::vector<uint32_t>& keys, const std::vector<uint32_t>& values, int groups, int iters) {
    if (groups <= 0) return 0;
    const uint32_t num_groups = static_cast<uint32_t>(groups);
    std::vector<uint64_t> agg(groups, 0);
    uint64_t out = 0;
    for (int iter = 0; iter < iters; ++iter) {
        std::fill(agg.begin(), agg.end(), 0ULL);
        const size_t n = keys.size();
        for (size_t i = 0; i < n; ++i) {
            const uint32_t k = keys[i];
            if (k < num_groups) {
                agg[k] += values[i];
            }
        }
        uint64_t hash = 1469598103934665603ULL;
        for (int g = 0; g < groups; ++g) {
            hash ^= agg[g];
            hash *= 1099511628211ULL;
        }
        out = hash;
    }
    return out;
}
