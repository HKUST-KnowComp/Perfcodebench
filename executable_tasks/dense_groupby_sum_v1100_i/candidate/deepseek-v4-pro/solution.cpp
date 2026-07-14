#include "interface.h"

#include <vector>
#include <algorithm>

uint64_t groupby_checksum(const std::vector<uint32_t>& keys, const std::vector<uint32_t>& values, int groups, int iters) {
    std::vector<uint64_t> sums(groups, 0);
    uint64_t out = 0;
    for (int iter = 0; iter < iters; ++iter) {
        // Clear the sums for the new iteration
        std::fill(sums.begin(), sums.end(), 0);

        // Aggregate only keys within [0, groups-1]
        for (size_t i = 0; i < keys.size(); ++i) {
            uint32_t k = keys[i];
            if (k < static_cast<uint32_t>(groups)) {
                sums[k] += static_cast<uint64_t>(values[i]);
            }
        }

        // Compute checksum identical to the original checksum_map logic
        uint64_t hash = 1469598103934665603ULL;
        for (int g = 0; g < groups; ++g) {
            hash ^= sums[g];
            hash *= 1099511628211ULL;
        }
        out = hash;
    }
    return out;
}