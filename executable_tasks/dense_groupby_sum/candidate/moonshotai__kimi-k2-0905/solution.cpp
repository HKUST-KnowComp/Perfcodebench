#include "interface.h"
#include <vector>
#include <cstdint>

namespace {

inline uint64_t checksum_vec(const std::vector<uint64_t>& agg) {
    const uint64_t* data = agg.data();
    const std::size_t n  = agg.size();
    uint64_t hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < n; ++i) {
        hash ^= data[i];
        hash *= 1099511628211ULL;
    }
    return hash;
}

}  // namespace

uint64_t groupby_checksum(const std::vector<uint32_t>& keys,
                          const std::vector<uint32_t>& values,
                          int groups,
                          int iters)
{
    std::vector<uint64_t> agg(groups, 0);
    const std::size_t N = keys.size();
    uint64_t out = 0;

    for (int iter = 0; iter < iters; ++iter) {
        std::fill(agg.begin(), agg.end(), 0);

        const uint32_t* k = keys.data();
        const uint32_t* v = values.data();
        for (std::size_t i = 0; i < N; ++i) {
            agg[k[i]] += static_cast<uint64_t>(v[i]);
        }

        out = checksum_vec(agg);
    }
    return out;
}