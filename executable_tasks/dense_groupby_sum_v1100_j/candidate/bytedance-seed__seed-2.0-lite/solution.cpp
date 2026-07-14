#include "interface.h"
#include <vector>
#include <algorithm>

uint64_t groupby_checksum(const std::vector<uint32_t>& keys, const std::vector<uint32_t>& values, int groups, int iters) {
    std::vector<uint64_t> sums(groups, 0);
    uint64_t out = 0;
    constexpr uint64_t fnv_prime = 1099511628211ULL;
    constexpr uint64_t fnv_offset = 1469598103934665603ULL;

    for (int iter = 0; iter < iters; ++iter) {
        std::fill(sums.begin(), sums.end(), 0);

        for (size_t i = 0; i < keys.size(); ++i) {
            sums[keys[i]] += static_cast<uint64_t>(values[i]);
        }

        uint64_t hash = fnv_offset;
        for (int g = 0; g < groups; ++g) {
            hash ^= sums[g];
            hash *= fnv_prime;
        }
        out = hash;
    }
    return out;
}