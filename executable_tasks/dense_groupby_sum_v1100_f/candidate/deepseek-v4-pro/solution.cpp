#include "interface.h"
#include <vector>
#include <algorithm>

namespace {

uint64_t checksum_array(const uint64_t* sums, int groups) {
    uint64_t hash = 1469598103934665603ULL;
    for (int g = 0; g < groups; ++g) {
        hash ^= sums[g];
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // anonymous namespace

uint64_t groupby_checksum(const std::vector<uint32_t>& keys, const std::vector<uint32_t>& values, int groups, int iters) {
    if (groups <= 0) {
        return 1469598103934665603ULL;
    }

    std::vector<uint64_t> sums(groups);
    uint64_t out = 0;
    for (int iter = 0; iter < iters; ++iter) {
        std::fill(sums.begin(), sums.end(), 0ULL);
        const size_t n = keys.size();
        for (size_t i = 0; i < n; ++i) {
            const uint32_t k = keys[i];
            if (k < static_cast<uint32_t>(groups)) {
                sums[k] += static_cast<uint64_t>(values[i]);
            }
        }
        out = checksum_array(sums.data(), groups);
    }
    return out;
}