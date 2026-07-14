#include "interface.h"
#include <cstdint>
#include <vector>

namespace {

inline uint64_t checksum_plain(const uint64_t* __restrict agg, int groups) {
    uint64_t hash = 1469598103934665603ULL;
    for (int g = 0; g < groups; ++g) {
        uint64_t v = agg[g];
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
    // Fast-path check: groups must be small enough to keep the array in L1/L2
    if (groups <= 0) return 0;

    // Re-usable buffer, zeroed once per iteration
    std::vector<uint64_t> agg(groups);

    uint64_t out = 0;
    const std::size_t n = keys.size();
    const uint32_t*  __restrict k = keys.data();
    const uint32_t*  __restrict v = values.data();

    for (int iter = 0; iter < iters; ++iter) {
        std::fill(agg.begin(), agg.end(), 0ULL);
        uint64_t* __restrict a = agg.data();

        for (std::size_t i = 0; i < n; ++i) {
            a[k[i]] += static_cast<uint64_t>(v[i]);
        }
        out = checksum_plain(a, groups);
    }
    return out;
}