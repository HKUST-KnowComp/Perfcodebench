#include "interface.h"

#include <cstdint>
#include <vector>
#include <cstring>

namespace {

inline uint64_t checksum_vec(const std::vector<uint64_t>& agg, int groups) {
    uint64_t hash = 1469598103934665603ULL;
    for (int g = 0; g < groups; ++g) {
        const uint64_t v = agg[g];
        hash ^= v;
        hash *= 1099511628211ULL;
    }
    return hash;
}

}  // namespace

uint64_t groupby_checksum(const std::vector<uint32_t>& keys,
                          const std::vector<uint32_t>& values,
                          int groups,
                          int iters) {
    std::vector<uint64_t> agg(groups, 0);
    const std::size_t n = keys.size();
    uint64_t out = 0;

    for (int iter = 0; iter < iters; ++iter) {
        // fast clear: zero the entire buffer
        std::memset(agg.data(), 0, groups * sizeof(uint64_t));

        // dense aggregation
        for (std::size_t i = 0; i < n; ++i) {
            agg[keys[i]] += static_cast<uint64_t>(values[i]);
        }

        out = checksum_vec(agg, groups);
    }
    return out;
}