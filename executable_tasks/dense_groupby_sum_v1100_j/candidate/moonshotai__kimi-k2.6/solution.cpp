#include "interface.h"

#include <algorithm>
#include <vector>

uint64_t groupby_checksum(const std::vector<uint32_t>& keys, const std::vector<uint32_t>& values, int groups, int iters) {
    std::vector<uint64_t> agg(groups);
    uint64_t out = 0;
    const size_t n = keys.size();
    const uint32_t* keys_data = keys.data();
    const uint32_t* values_data = values.data();

    for (int iter = 0; iter < iters; ++iter) {
        std::fill(agg.begin(), agg.end(), 0);
        uint64_t* agg_data = agg.data();

        for (size_t i = 0; i < n; ++i) {
            agg_data[keys_data[i]] += static_cast<uint64_t>(values_data[i]);
        }

        uint64_t hash = 1469598103934665603ULL;
        for (int g = 0; g < groups; ++g) {
            hash ^= agg_data[g];
            hash *= 1099511628211ULL;
        }
        out = hash;
    }

    return out;
}
